
#include "stdafx.h"
#include "TcpSocketEventDispatcher.h"
#include "TcpSocketServer.h"

#include <WSAHelper.h>
#include <IConnectionManager.h>
#include <SocketServer.h>
#include <Socket.h>
#include <IOBuffer.h>
#include <ISocketEvent.h>
#include <Macros.h>
#include <Statistics.h>
#include <log4cplus\loggingmacros.h>
#include <Configurations.h>

using namespace log4cplus;

namespace TcpSocketServerSide
{

	TcpSocketEventDispatcher::TcpSocketEventDispatcher(SocketServer& svr)
		:server(svr), isRunning(false)
	{
	}

	TcpSocketEventDispatcher::~TcpSocketEventDispatcher(void)
	{
		auto first = this->handlers.begin();
		for(; first != this->handlers.end(); ++first){
			delete *first;
		}
	}

	void TcpSocketEventDispatcher::UpdateSocket(Socket& skt, IOBuffer& buffer){
		SOCKET sServer =
			static_cast<TcpSocketServer&>(this->server).AsSocket();

		int ret = setsockopt(skt.AsSocket(), 
			SOL_SOCKET, 
			SO_UPDATE_ACCEPT_CONTEXT, 
			(char *)&sServer, 
			sizeof(sServer) 
			);

		if(SOCKET_ERROR == ret){
			ThrowWSAErrorMessage();
		}

		// parse the buffer
		SOCKADDR *lpLocalSockaddr = nullptr, *lpRemoteSockaddr = nullptr; 
		int LocalSockaddrLen = 0, RemoteSockaddrLen = 0;

		WSAExtMethods::GetAcceptExSockaddrs(
			buffer.GetPointer(),
			buffer.GetSize() - ((sizeof(sockaddr_in) + 16) * 2),
			sizeof(SOCKADDR_IN)+16, 
			sizeof(SOCKADDR_IN)+16, 
			&lpLocalSockaddr, 
			&LocalSockaddrLen, 
			&lpRemoteSockaddr, 
			&RemoteSockaddrLen
			);

		memcpy(skt.GetPeerAddress(), lpRemoteSockaddr, RemoteSockaddrLen);

	}

	void TcpSocketEventDispatcher::Process(){
		IOCompletionPort& iocp = this->server.GetIOCompletionPort();
		IConnectionManager& monitor = this->server.GetConnectionManager();
		DWORD numberOfBytes = 0;
		Socket* socket = nullptr;
		IOBuffer* buffer = nullptr;

		this->isRunning = true;

		while (true)
		{
			bool isSuccess = iocp.GetQueuedCompletionStatus(
				&numberOfBytes, &socket,
				&buffer, INFINITE
				);

			if(!isSuccess){
				// TODO: write log
				DWORD err = ::GetLastError();

				if(ERROR_NETNAME_DELETED == err){
					::InterlockedDecrement(&Statistics::PendingReads);
					socket->OnReadZeroByte(buffer);
					continue;
				}

				LOG4CPLUS_FATAL(
					Configurations::GetLogger(), 
					_T("GetQueuedCompletionStatus(...) failed. ErrorCode: ") << err
					);
				break;
			}

			if(buffer->GetOperation() == IO_Termination){
				break;
			}

			switch (buffer->GetOperation())
			{

			case IO_Accept_Completed:
				{
					if(this->server.IsShutting()){
						socket->DecreaseRef();
						delete buffer;
						break;
					}

					socket = &buffer->GetSocket();
					iocp.AssociateSocket(socket->AsSocket(), socket);


					if(0 == numberOfBytes){
						buffer->SetOperation(IO_Disconnect_Completed);

						int ret = WSAExtMethods::DisconnectEx(
							socket->AsSocket(), 
							buffer, 
							TF_REUSE_SOCKET, 0);

						if(TRUE == ret ){
							monitor.OnDisconnected(socket, buffer);
						}
						else{
							if(WSA_IO_PENDING != WSAGetLastError()){
								// TODO: write log
								LOG4CPLUS_FATAL(
									Configurations::GetLogger(), 
									_T("IO_Accept_Completed->DisconnectEx(...) failed. ErrorCode: ")
									<< WSAGetLastError()
									);
								socket->DecreaseRef();
								delete buffer;
							}
						}
					}
					else{
						this->UpdateSocket(*socket, *buffer);
						buffer->SetUsed(numberOfBytes + buffer->GetUsed());
						this->TriggerPostAccept(*socket, *buffer);
					}

				}
				break;

			case IO_Disconnect_Completed:
				{
					monitor.OnDisconnected(socket, buffer);
				}
				break;

			case IO_Read_Completed:
				{
					::InterlockedDecrement(&Statistics::PendingReads);

					if(0 == numberOfBytes){
						socket->OnReadZeroByte(buffer);
					}
					else{
						buffer->SetUsed(numberOfBytes + buffer->GetUsed());
						this->TriggerPostRead(*socket, *buffer);
					}
				}
				break;

			case IO_Write_Completed:
				{
					::InterlockedDecrement(&Statistics::PendingWrites);

					this->TriggerPostWrite(*socket, *buffer);
				}
				break;

			default:
				{
					// TODO: write log

					LOG4CPLUS_FATAL(
						Configurations::GetLogger(), 
						_T("Unidentified data. Operation type: ") << (int)buffer->GetOperation()
						);

					if(nullptr != socket){
						socket->DecreaseRef();
					}

					delete buffer;
				}
				break;
			} // end of switch

			numberOfBytes = 0;
			socket = nullptr;
			buffer = nullptr;
		} // end of while


		::InterlockedDecrement(&Statistics::AliveTask);
		if(!this->server.IsShutting()){
			LOG4CPLUS_FATAL(
				Configurations::GetLogger(), 
				_T("IO process thread exit unexpected . ErrorCode: ") << GetLastError()
				);
		}
	}

	void TcpSocketEventDispatcher::Register(ISocketEvent* handler){

		if(this->isRunning){
			throw std::exception("Do not call Register(...) when the server is running.");
		}

		if(nullptr == handler){
			throw std::exception("The argument 'handler' can not be null");
		}

		this->handlers.push_back(handler);
	}

	void TcpSocketEventDispatcher::TriggerPreAccept(Socket& socket, IOBuffer& buffer){
		for(auto first = this->handlers.begin(); first != this->handlers.end(); ++first){
			try{
				(*first)->PreAccept(socket, buffer);
			}
			catch(std::exception &ex){
				// TODO: write log
				wchar_t* wcs = (wchar_t*)malloc(4*1024);
				std::string exStr(ex.what());
				mbstowcs(wcs, ex.what(), exStr.length());
				LOG4CPLUS_ERROR(Configurations::GetLogger(), wcs);
				free(wcs);
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), "Exception at TriggerPreAccept(...)");
			}
		}
	}

	void TcpSocketEventDispatcher::TriggerPostAccept(Socket& socket, IOBuffer& buffer){
		for(auto first = this->handlers.begin(); first != this->handlers.end(); ++first){
			try{
				(*first)->PostAccept(socket, buffer);
			}
			catch(std::exception &ex){
				// TODO: write log
				wchar_t* wcs = (wchar_t*)malloc(4*1024);
				std::string exStr(ex.what());
				mbstowcs(wcs, ex.what(), exStr.length());
				LOG4CPLUS_ERROR(Configurations::GetLogger(), wcs);
				free(wcs);
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), "Exception at TriggerPostAccept(...)");
			}
		}
	}

	void TcpSocketEventDispatcher::TriggerError(Socket& socket){
		for(auto first = this->handlers.begin(); first != this->handlers.end(); ++first){
			try{
				(*first)->OnError(socket);
			}
			catch(std::exception &ex){
				// TODO: write log
				wchar_t* wcs = (wchar_t*)malloc(4*1024);
				std::string exStr(ex.what());
				mbstowcs(wcs, ex.what(), exStr.length());
				LOG4CPLUS_ERROR(Configurations::GetLogger(), wcs);
				free(wcs);
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), "Exception at TriggerError(...)");
			}
		}
	}

	void TcpSocketEventDispatcher::TriggerPostClose(Socket& socket){
		for(auto first = this->handlers.begin(); first != this->handlers.end(); ++first){
			try{
				(*first)->PostClose(socket);
			}
			catch(std::exception &ex){
				// TODO: write log
				wchar_t* wcs = (wchar_t*)malloc(4*1024);
				std::string exStr(ex.what());
				mbstowcs(wcs, ex.what(), exStr.length());
				LOG4CPLUS_ERROR(Configurations::GetLogger(), wcs);
				free(wcs);
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), "Exception at TriggerPostClose(...)");
			}
		}
	}

	void TcpSocketEventDispatcher::TriggerPostRead(Socket& socket, IOBuffer& buffer){
		for(auto first = this->handlers.begin(); first != this->handlers.end(); ++first){
			try{
				(*first)->PostRead(socket, buffer);
			}
			catch(std::exception &ex){
				// TODO: write log
				wchar_t* wcs = (wchar_t*)malloc(4*1024);
				std::string exStr(ex.what());
				mbstowcs(wcs, ex.what(), exStr.length());
				LOG4CPLUS_ERROR(Configurations::GetLogger(), wcs);
				free(wcs);
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), "Exception at TriggerPostRead(...)");
			}
		}
	}

	void TcpSocketEventDispatcher::TriggerPostWrite(Socket& socket, IOBuffer& buffer){
		for(auto first = this->handlers.begin(); first != this->handlers.end(); ++first){
			try{
				(*first)->PostWrite(socket, buffer);
			}
			catch(std::exception &ex){
				// TODO: write log
				wchar_t* wcs = (wchar_t*)malloc(4*1024);
				std::string exStr(ex.what());
				mbstowcs(wcs, ex.what(), exStr.length());
				LOG4CPLUS_ERROR(Configurations::GetLogger(), wcs);
				free(wcs);
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), "Exception at TriggerPostWrite(...)");
			}
		}
	}

	void TcpSocketEventDispatcher::TriggerPreClose(Socket& socket){
		for(auto first = this->handlers.begin(); first != this->handlers.end(); ++first){
			try{
				(*first)->PreClose(socket);
			}
			catch(std::exception &ex){
				// TODO: write log
				wchar_t* wcs = (wchar_t*)malloc(4*1024);
				std::string exStr(ex.what());
				mbstowcs(wcs, ex.what(), exStr.length());
				LOG4CPLUS_ERROR(Configurations::GetLogger(), wcs);
				free(wcs);
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), "Exception at TriggerPreClose(...)");
			}
		}
	}

	void TcpSocketEventDispatcher::TriggerPreRead(Socket& socket, IOBuffer& buffer){
		for(auto first = this->handlers.begin(); first != this->handlers.end(); ++first){
			try{
				(*first)->PreRead(socket, buffer);
			}
			catch(std::exception &ex){
				// TODO: write log
				wchar_t* wcs = (wchar_t*)malloc(4*1024);
				std::string exStr(ex.what());
				mbstowcs(wcs, ex.what(), exStr.length());
				LOG4CPLUS_ERROR(Configurations::GetLogger(), wcs);
				free(wcs);
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), "Exception at TriggerPreRead(...)");
			}
		}
	}

	void TcpSocketEventDispatcher::TriggerPreWrite(Socket& socket, IOBuffer& buffer){
		for(auto first = this->handlers.begin(); first != this->handlers.end(); ++first){
			try{
				(*first)->PreWrite(socket, buffer);
			}
			catch(std::exception &ex){
				// TODO: write log
				wchar_t* wcs = (wchar_t*)malloc(4*1024);
				std::string exStr(ex.what());
				mbstowcs(wcs, ex.what(), exStr.length());
				LOG4CPLUS_ERROR(Configurations::GetLogger(), wcs);
				free(wcs);
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), "Exception at TriggerPreWrite(...)");
			}
		}
	}


}