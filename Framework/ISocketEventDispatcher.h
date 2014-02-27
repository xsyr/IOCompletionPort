

#ifndef _ISocketEventDispatcher_H_
#define _ISocketEventDispatcher_H_

#include <IOBuffer.h>
#include <Framework.h>
#include <Uncopyable.h>

namespace Framework
{

	class ISocketEventDispatcher : private Uncopyable{
	public:
		virtual void Process() = 0;
		virtual void Register(ISocketEvent* handler) = 0;

		virtual void TriggerPreAccept(Socket& socket, IOBuffer& buffer) = 0;
		virtual void TriggerPostAccept(Socket& socket, IOBuffer& buffer) = 0;
		virtual void TriggerError(Socket& socket) = 0;
		virtual void TriggerPostClose(Socket& socket) = 0;
		virtual void TriggerPostRead(Socket& socket, IOBuffer& buffer) = 0;
		virtual void TriggerPostWrite(Socket& socket, IOBuffer& buffer) = 0;
		virtual void TriggerPreClose(Socket& socket) = 0;
		virtual void TriggerPreRead(Socket& socket, IOBuffer& buffer) = 0;
		virtual void TriggerPreWrite(Socket& socket, IOBuffer& buffer) = 0;

		virtual ~ISocketEventDispatcher() {}
	};

}

#endif