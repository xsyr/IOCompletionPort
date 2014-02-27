
#ifndef _ITaskFactory_H_
#define _ITaskFactory_H_

#include <Framework.h>
#include <Uncopyable.h>

namespace Framework
{

	class ITaskFactory : private Uncopyable{
	public:
		virtual unsigned int Create(ISocketEventDispatcher& dispatcher, SocketServer& server) = 0;
		virtual ~ITaskFactory(){ }
	};


}
#endif