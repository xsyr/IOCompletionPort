
#ifndef _DefaultSocketEventDispatcher_H_
#define _DefaultSocketEventDispatcher_H_

#include <Framework.h>
#include <vector>
#include <ISocketEventDispatcher.h>


namespace Framework
{

	class DefaultSocketEventDispatcher : public ISocketEventDispatcher
	{
	private:
		SocketServer& server;
		std::vector<ISocketEvent*> handlers;
		bool isRunning;

		void UpdateSocket(Socket& skt, IOBuffer& buffer);

	public:
		DefaultSocketEventDispatcher(SocketServer& svr);
		virtual ~DefaultSocketEventDispatcher(void);

		virtual void Process();
		virtual void Register(ISocketEvent* handler);
		
		virtual void TriggerPreAccept(Socket& socket, IOBuffer& buffer);
		virtual void TriggerPostAccept(Socket& socket, IOBuffer& buffer);
		virtual void TriggerError(Socket& socket);
		virtual void TriggerPostClose(Socket& socket);
		virtual void TriggerPostRead(Socket& socket, IOBuffer& buffer);
		virtual void TriggerPostWrite(Socket& socket, IOBuffer& buffer);
		virtual void TriggerPreClose(Socket& socket);
		virtual void TriggerPreRead(Socket& socket, IOBuffer& buffer);
		virtual void TriggerPreWrite(Socket& socket, IOBuffer& buffer);
	};

}

#endif