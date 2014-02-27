
#ifndef _IServerEvent_H_
#define _IServerEvent_H_

#include <Framework.h>
#include <Uncopyable.h>

namespace Framework
{

	class IServerEvent : private Uncopyable{
	public:
		virtual void OnError() = 0;
		virtual void PostShutdown() = 0;
		virtual void PostStart() = 0;
		virtual void PreShutdown() = 0;
		virtual void PreStart() = 0;
	};
}

#endif