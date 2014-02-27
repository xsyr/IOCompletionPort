#include "stdafx.h"
#include "TcpSocketServer.h"
#include <Macros.h>

namespace TcpSocketServerSide
{

	TcpSocketServer::TcpSocketServer(
		char* hostname, 
		unsigned short port,
		Framework::ITaskFactory* factory, 
		DWORD nConcurrentThreads)
		:
	SocketServer(factory, nConcurrentThreads)
	{
		using namespace Framework;

		memset(&this->address, 0, sizeof(this->address));
		this->address.sin_family = AF_INET;
		this->address.sin_addr.S_un.S_addr = inet_addr(hostname);
		if(INADDR_NONE == this->address.sin_addr.S_un.S_addr){
			throw std::exception("Invalid hostname");
		}

		this->socket = WSAUtility::CreateOverlappedSocket(WSAUtility::TCP());
		if(INVALID_SOCKET == this->socket){
			ThrowWSAErrorMessage();
		}


		BOOL bReuse = TRUE;
		int ret = setsockopt(
			this->socket, 
			SOL_SOCKET, 
			SO_REUSEADDR, 
			(const char *)&bReuse, 
			sizeof(bReuse));

		if(SOCKET_ERROR == ret){
			ThrowWSAErrorMessage();
		}


		this->address.sin_port = htons(port);
	}


	TcpSocketServer::~TcpSocketServer(void)
	{
		::closesocket(this->socket);
		this->socket = INVALID_SOCKET;
	}


	void TcpSocketServer::OnStartup(){
		
		using namespace Framework;

		if(SOCKET_ERROR == 
			::bind(this->socket, (sockaddr*)&this->address, sizeof(this->address))
			){
				ThrowWSAErrorMessage();
		}

		if(SOCKET_ERROR == ::listen(this->socket, SOMAXCONN)){
			ThrowWSAErrorMessage();
		}

		if(!this->iocp.AssociateSocket(this->socket, 0)){
			ThrowNTErrorMessage();
		}
	}

}