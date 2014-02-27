#include "stdafx.h"
#include <DefaultTaskFactory.h>
#include <ISocketEventDispatcher.h>

namespace Framework
{

	DefaultTaskFactory::DefaultTaskFactory(unsigned int nTask)
		:numberOfTask(nTask)
	{
		if(0 == this->numberOfTask){
			SYSTEM_INFO info;
			memset(&info, 0, sizeof(info));

			GetSystemInfo(&info);

			this->numberOfTask = info.dwNumberOfProcessors * 2;
		}
	}


	DefaultTaskFactory::~DefaultTaskFactory(void)
	{
	}

	void DefaultTaskFactory::RunDispatcher(void* dsp){
		ISocketEventDispatcher* dispatcher = 
			static_cast<ISocketEventDispatcher*>(dsp);

		dispatcher->Process();
	}

	unsigned int DefaultTaskFactory::Create(
		ISocketEventDispatcher& dispatcher, 
		SocketServer& server)
	{

		for (int i = 0; i < this->numberOfTask; i++)
		{
			_beginthread(DefaultTaskFactory::RunDispatcher, 0, (void*)&dispatcher);
		}

		return numberOfTask;
	}
}