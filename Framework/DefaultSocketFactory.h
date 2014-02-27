
#ifndef _DefaultSocketFactory_H_
#define _DefaultSocketFactory_H_

#include <Framework.h>
#include <ISocketFactory.h>

namespace Framework
{

	class DefaultSocketFactory :
		public ISocketFactory
	{
	public:
		Socket* Create( 
			SocketServer& server, 
			ISocketEventDispatcher& dispatcher);
	};

}

#endif