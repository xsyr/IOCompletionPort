#include "stdafx.h"
#include <DefaultConnectionManager.h>
#include <ISocketEventDispatcher.h>
#include <ISocketFactory.h>
#include <WSAHelper.h>
#include <Socket.h>
#include <SocketServer.h>
#include <IOBuffer.h>
#include <Statistics.h>
#include <Configurations.h>

namespace Framework
{

	DefaultConnectionManager
		::DefaultConnectionManager(
			SocketServer& svr, 
			ISocketEventDispatcher& dsp,
			ISocketFactory& factory)
		: server(svr), dispatcher(dsp), factory(factory)
	{
		dsp.Register(new DefaultConnectionManager::SocketEventHandler(*this));
	}

	DefaultConnectionManager::~DefaultConnectionManager(void)
	{
	}

	void DefaultConnectionManager::OnDisconnected(Socket* socket, IOBuffer* buffer){
		buffer->Clear();
		this->PostAcceptRequest(socket, buffer);
	}

	void DefaultConnectionManager::Process(SocketServer& server){
		this->PostAcceptRequest();
	}

	bool DefaultConnectionManager::PostAcceptRequest(){
		Socket* s = this->factory.Create(this->server, this->dispatcher);
		IOBuffer* buf = 
			new(Configurations::BufferSize)
			IOBuffer(Configurations::BufferSize, *s);

		return this->PostAcceptRequest(s, buf);
	}

	bool DefaultConnectionManager::PostAcceptRequest(Socket* s, IOBuffer* buf){

		DWORD bytes = 0;
		BOOL isCompleted = FALSE;

		buf->SetOperation(IO_Accept_Completed);

		do{
			this->dispatcher.TriggerPreAccept(*s, *buf);

			isCompleted = WSAExtMethods::AcceptEx(
				this->server.AsSocket(),
				s->AsSocket(),
				(*buf)[0],
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


	void DefaultConnectionManager
		::SocketEventHandler
		::PostAccept(Socket& socket, IOBuffer& buffer)
	{
		InterlockedIncrement(&Statistics::ConnectingSockets);
		this->monitor.PostAcceptRequest();
	}

	void DefaultConnectionManager
		::SocketEventHandler
		::PreAccept(Socket& socket, IOBuffer& buffer)
	{

	}

	void DefaultConnectionManager
		::SocketEventHandler
		::PostClose(Socket& socket)
	{
		::InterlockedDecrement(&Statistics::ConnectingSockets);
	}
}