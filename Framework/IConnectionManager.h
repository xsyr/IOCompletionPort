
#ifndef _IConnectionManager_H_
#define _IConnectionManager_H_

#include <Framework.h>
#include <ISocketEvent.h>
#include <Uncopyable.h>

namespace Framework
{
	class IConnectionManager : private Uncopyable
	{
	public:
		virtual void OnDisconnected(Socket* socket, IOBuffer* buffer) = 0;
		virtual void Process(SocketServer& server) = 0;
		virtual ~IConnectionManager(){}
	};

}

#endif