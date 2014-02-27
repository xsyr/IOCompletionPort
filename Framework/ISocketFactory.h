
#ifndef _ISocketFactory_H_
#define _ISocketFactory_H_

#include <Framework.h>

namespace Framework
{
	class ISocketFactory
	{
	public:
		virtual Socket* Create(
			SocketServer& server, 
			ISocketEventDispatcher& dispatcher) = 0;
	};

}

#endif