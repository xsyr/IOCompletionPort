
#include "stdafx.h"
#include <Socket.h>
#include <ISocketEventDispatcher.h>
#include <IOCompletionPort.h>
#include <SocketServer.h>
#include <Macros.h>
#include <SocketClosedException.h>
#include <ServerClosedException.h>
#include <Statistics.h>
#include <Configurations.h>

namespace Framework
{

	char* Socket::StatusDescription[] = 
	{
		"The socket deny receive.",
		"The socket deny send.",
		"The socket deny receive and send.",
		"The socket is closed."
	};

	Socket::Socket(
		SOCKET skt, 
		SocketServer& svr, 
		ISocketEventDispatcher& dispatcher, 
		const sockaddr_in& addr)
		:
	peer(addr), 
		server(svr),
		dispatcher(dispatcher),
		pendingReads(0),
		reference(1),
		isClosed(FALSE)
	{

		this->Init(skt, svr);
	}

	Socket::Socket(
		SOCKET skt, 
		SocketServer& svr,
		ISocketEventDispatcher& dispatcher)
		:
	server(svr),
		dispatcher(dispatcher),
		pendingReads(0),
		reference(1),
		isClosed(FALSE)
	{
		this->Init(skt, svr);
	}

	Socket::~Socket() {
		if(Closed != this->status){
			::closesocket(this->socket);
		}

		while (!this->pendingWrites.IsEmpty())
		{
			IOBuffer* buf = this->pendingWrites.Dequeue();
			try{
				delete buf;
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), _T("Exception when 'delete buf'."));
			}
		}
	}

	void Socket::Init(SOCKET socket, SocketServer& svr){
		using std::exception;

		if(INVALID_SOCKET == socket){
			throw exception("Invalid socket");
		}

		status = Normal;
		this->socket = socket;
		this->iocp = &svr.GetIOCompletionPort();

		memset(&this->peer, 0, sizeof(this->peer));
	}

	bool Socket::IsConnected(unsigned long& time){
		bool connected = false;

		if(INVALID_SOCKET == this->socket 
			|| Closed == this->status){
				connected = false;
		}
		else{
			int size = sizeof(time);

			int ret = getsockopt(
				this->socket, 
				SOL_SOCKET, 
				SO_CONNECT_TIME,
				reinterpret_cast<char*>(&time),
				&size
				);

			if(SOCKET_ERROR == ret){
				connected = false;
			}
			else{
				connected = 0xFFFFFFFF != time;
			}
		}

		if(!connected){
			time = 0L;
		}

		return connected;
	}

	void Socket::OnReadZeroByte(IOBuffer* buffer){
		if(0 == ::InterlockedDecrement(&this->pendingReads)){
			this->Close();
		}

		delete buffer;
	}

	void Socket::Close(){
		if(FALSE == ::InterlockedCompareExchange(&this->isClosed, TRUE, FALSE)){
			this->dispatcher.TriggerPreClose(*this);
			::closesocket(this->socket);
			this->status = Closed;
			try{
				this->dispatcher.TriggerPostClose(*this);
			}
			catch(...){
				// TODO: write log
				LOG4CPLUS_ERROR(Configurations::GetLogger(), _T(" Exception at Close"));
			}

			this->DecreaseRef();
		}
	}

	void Socket::RegisterEventHandler(ISocketEventDispatcher& dispatcher){
		dispatcher.Register(new Socket::SocketEventHandler());
	}

	bool Socket::Read(IOBuffer* buffer){

		if(nullptr == buffer){
			throw std::exception("The argument 'buffer' cannot be null.");
		}

		if(this->status == Closed 
			|| this->status == DenyReceive 
			|| this->status == DenySendnReceive){
				throw SocketClosedException(StatusDescription[this->status]);
		}

		// check server status
		if(this->server.IsShutting()){
			throw ServerClosedException();
		}

		DWORD numberOfBytesRecvd;
		DWORD flag = 0;

		buffer->SetOperation(IO_Read_Completed);
		this->dispatcher.TriggerPreRead(*this, *buffer);
		WSABUF buf = 
		{ 
			buffer->GetSize() - buffer->GetUsed(),
			(char*)buffer->GetPointer(buffer->GetUsed())
		};


		int ret = WSARecv(
			this->socket, 
			&buf, 
			1, 
			&numberOfBytesRecvd, 
			&flag,
			static_cast<WSAOVERLAPPED*>(buffer),
			nullptr
			);

		int err = ERROR_SUCCESS;

		if(SOCKET_ERROR == ret && WSA_IO_PENDING != WSAGetLastError()){
			err = WSAGetLastError();
			//ThrowWSAErrorMessage();
		}

		return 0 == ret;
	}

	bool Socket::DoWrite(IOBuffer* buffer){
		if(this->status == Closed 
			|| this->status == DenySend 
			|| this->status == DenySendnReceive){
				throw SocketClosedException(StatusDescription[this->status]);
		}

		buffer->SetOperation(IO_Write_Completed);

		this->dispatcher.TriggerPreWrite(*this, *buffer);

		DWORD sentBytes;
		WSABUF buf = { buffer->GetUsed(), (char*)buffer->GetPointer()};

		int ret = WSASend(
			this->socket,
			&buf,
			1, &sentBytes, 0,
			static_cast<WSAOVERLAPPED*>(buffer),
			NULL
			);

		if(SOCKET_ERROR == ret 
			&& WSA_IO_PENDING != WSAGetLastError()){
				ThrowWSAErrorMessage();
		}

		return 0 == ret;
	}

	void Socket::Shutdown(int how){

		if(DenyReceive != how
			&& DenySend != how
			&& DenySendnReceive != how
			){
				throw std::exception("Undefine operation");
		}


		this->status = static_cast<SocketStatus>(how);
		::shutdown(this->socket, how);
	}

	void Socket::Write(IOBuffer* buffer){
		if(nullptr == buffer){
			throw std::exception("The argument 'buffer' cannot be null.");
		}

		QueueLock::Owner ow(this->pendingWrites.GetLock());

		// check server status
		if(this->server.IsShutting()){
			throw ServerClosedException();
		}

		if(this->pendingWrites.IsEmpty()){
			this->DoWrite(buffer);
		}
		else{
			this->pendingWrites.Enqueue(buffer);
		}
	}

	void Socket::Write(const std::vector<IOBuffer*>& buffers){
		if(buffers.empty()){
			throw std::exception("The argument 'buffers' cannot be empty.");
		}

		// lock
		QueueLock::Owner ow(this->pendingWrites.GetLock());

		// check server status
		if(this->server.IsShutting()){
			throw ServerClosedException();
		}

		auto first = buffers.begin();

		if(this->pendingWrites.IsEmpty()){
			this->DoWrite(*first);
			++first;
		}

		while (first != buffers.end()){
			this->pendingWrites.Enqueue(*first);
			++first;
		}
	}

	void Socket::SocketEventHandler::PostWrite(Socket& socket, IOBuffer& buffer){
		QueueLock::Owner ow(socket.pendingWrites.GetLock());
		if(!socket.server.IsShutting() && !socket.pendingWrites.IsEmpty()){
			socket.DoWrite(socket.pendingWrites.Dequeue());
		}
	}

	void Socket::SocketEventHandler::PostRead(Socket& socket, IOBuffer& buffer){
		::InterlockedDecrement(&socket.pendingReads);
	}

	void Socket::SocketEventHandler::PreRead(Socket& socket, IOBuffer& buffer){
		::InterlockedIncrement(&socket.pendingReads);
		::InterlockedIncrement(&Statistics::PendingReads);
	}

	void Socket::SocketEventHandler::PreWrite(Socket& socket, IOBuffer& buffer){
		::InterlockedIncrement(&Statistics::PendingWrites);
	}

	void Socket::SocketEventHandler::PostClose(Socket& socket){

	}
}