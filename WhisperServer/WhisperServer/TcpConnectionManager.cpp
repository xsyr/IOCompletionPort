#include "stdafx.h"
#include "TcpConnectionManager.h"
#include "TcpSocketServer.h"
#include <ISocketEventDispatcher.h>
#include <ISocketFactory.h>
#include <WSAHelper.h>
#include <Socket.h>
#include <SocketServer.h>
#include <IOBuffer.h>
#include <Statistics.h>
#include <Configurations.h>

namespace TcpSocketServerSide
{
	using namespace Framework;

	TcpConnectionManager
		::TcpConnectionManager(
			SocketServer& svr, 
			ISocketEventDispatcher& dsp,
			ISocketFactory& factory)
		: server(svr), dispatcher(dsp), factory(factory)
	{
		dsp.Register(new TcpConnectionManager::SocketEventHandler(*this));
	}

	TcpConnectionManager::~TcpConnectionManager(void)
	{
	}

	void TcpConnectionManager::OnDisconnected(Socket* socket, IOBuffer* buffer){
		buffer->Clear();
		this->PostAcceptRequest(socket, buffer);
	}

	void TcpConnectionManager::Process(SocketServer& server){
		this->PostAcceptRequest();
	}

	bool TcpConnectionManager::PostAcceptRequest(){
		Socket* s = this->factory.Create(this->server, this->dispatcher);
		IOBuffer* buf = 
			new(Configurations::BufferSize)
			IOBuffer(Configurations::BufferSize, *s);

		return this->PostAcceptRequest(s, buf);
	}

	bool TcpConnectionManager::PostAcceptRequest(Socket* s, IOBuffer* buf){

		DWORD bytes = 0;
		BOOL isCompleted = FALSE;

		buf->SetOperation(IO_Accept_Completed);

		do{
			this->dispatcher.TriggerPreAccept(*s, *buf);

			isCompleted = WSAExtMethods::AcceptEx(
				static_cast<TcpSocketServer&>(server).AsSocket(),
				s->AsSocket(),
				buf->GetPointer(),
				buf->GetSize() - ((sizeof(sockaddr_in) + 16) * 2),
				sizeof(sockaddr_in) + 16,
				sizeof(sockaddr_in) + 16,
				&bytes,
				static_cast<LPOVERLAPPED>(buf)
				);

			if(isCompleted){
				this->server.GetIOCompletionPort()
					.PostQueuedCompletionStatus(bytes, s, buf);

				break;
			}
			else{
				if(WSAECONNRESET == WSAGetLastError()){
					continue;
				}
				else if(WSA_IO_PENDING == WSAGetLastError()){
					break;
				}
				else{
					// TODO: write log
					int err = WSAGetLastError();
					s->DecreaseRef();
					delete buf;
				}
			}

		}while(true);

		return isCompleted == TRUE;
	}


	void TcpConnectionManager
		::SocketEventHandler
		::PostAccept(Socket& socket, IOBuffer& buffer)
	{
		InterlockedIncrement(&Statistics::ConnectingSockets);
		this->monitor.PostAcceptRequest();
	}

	void TcpConnectionManager
		::SocketEventHandler
		::PreAccept(Socket& socket, IOBuffer& buffer)
	{

	}

	void TcpConnectionManager
		::SocketEventHandler
		::PostClose(Socket& socket)
	{
		::InterlockedDecrement(&Statistics::ConnectingSockets);
	}
}