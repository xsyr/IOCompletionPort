

#ifndef _DefaultConnectionManager_H_
#define _DefaultConnectionManager_H_

#include <Framework.h>
#include <IConnectionManager.h>

namespace Framework
{

	class DefaultConnectionManager : public IConnectionManager
	{
	private:
		class SocketEventHandler : public ISocketEvent{
		private:
			DefaultConnectionManager& monitor;

		public:
			SocketEventHandler(DefaultConnectionManager& mtr) : monitor(mtr)
			{}

			virtual void PreAccept(Socket& socket, IOBuffer& buffer);
			virtual void PostAccept(Socket& socket, IOBuffer& buffer);
			virtual void OnError(Socket& socket){}
			virtual void PostClose(Socket& socket);
			virtual void PostRead(Socket& socket, IOBuffer& buffer){}
			virtual void PostWrite(Socket& socket, IOBuffer& buffer){}
			virtual void PreClose(Socket& socket){}
			virtual void PreRead(Socket& socket, IOBuffer& buffer){}
			virtual void PreWrite(Socket& socket, IOBuffer& buffer){}
		};

		ISocketEventDispatcher& dispatcher;
		SocketServer& server;
		ISocketFactory& factory;

		bool PostAcceptRequest();
		bool PostAcceptRequest(Socket* socket, IOBuffer* buffer);

	public:
		DefaultConnectionManager(
			SocketServer& svr, 
			ISocketEventDispatcher& dsp,
			ISocketFactory& factory);
		~DefaultConnectionManager(void);

		virtual void OnDisconnected(Socket* socket, IOBuffer* buffer);
		virtual void Process(SocketServer& server);
	};

}

#endif