
#include "stdafx.h"
#include "ITaskFactory.h"

namespace Framework
{
	class TaskFactory : public ITaskFactory
	{
		virtual unsigned int Create(ISocketEventDispatcher& dispatcher, SocketServer& server){
			server.~SocketServer();
			return 0;
		}
	};

}