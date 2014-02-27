

#ifndef _ISocketEvent_H_
#define _ISocketEvent_H_

#include <IOBuffer.h>
#include <Framework.h>
#include <Uncopyable.h>


namespace Framework
{

	class ISocketEvent : private Uncopyable{
	public:
		virtual void PreAccept(Socket& socket, IOBuffer& buffer) = 0;
		virtual void PostAccept(Socket& socket, IOBuffer& buffer) = 0;
		virtual void OnError(Socket& socket) = 0;
		virtual void PostClose(Socket& socket) = 0;
		virtual void PostRead(Socket& socket, IOBuffer& buffer) = 0;
		virtual void PostWrite(Socket& socket, IOBuffer& buffer) = 0;
		virtual void PreClose(Socket& socket) = 0;
		virtual void PreRead(Socket& socket, IOBuffer& buffer) = 0;
		virtual void PreWrite(Socket& socket, IOBuffer& buffer) = 0;

		virtual ~ISocketEvent(){ }
	};

}

#endif