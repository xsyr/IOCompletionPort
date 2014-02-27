
#ifndef _TcpSocketServer_H_
#define _TcpSocketServer_H_

#include <SocketServer.h>

namespace TcpSocketServerSide
{

	class TcpSocketServer :
		public Framework::SocketServer
	{
	private:
		sockaddr_in address;
		SOCKET socket;

	protected:
		virtual void OnStartup();

	public:
		TcpSocketServer(
			char* hostname, 
			unsigned short port,
			Framework::ITaskFactory* factory, 
			DWORD nConcurrentThreads = 0);
		~TcpSocketServer(void);


		SOCKET AsSocket() const{
			return this->socket;
		}

	};

}

#endif