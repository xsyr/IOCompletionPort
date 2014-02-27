
#ifndef _TcpSocketFactory_H_
#define _TcpSocketFactory_H_

#include <Framework.h>
#include <ISocketFactory.h>

namespace TcpSocketServerSide
{
	using namespace Framework;

	class TcpSocketFactory :
		public ISocketFactory
	{
	public:
		Socket* Create( 
			SocketServer& server, 
			ISocketEventDispatcher& dispatcher);
	};

}

#endif