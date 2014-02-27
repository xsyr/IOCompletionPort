
#ifndef _DefaultTaskFactory_H_
#define _DefaultTaskFactory_H_

#include <Framework.h>
#include <ITaskFactory.h>

namespace Framework
{

	class DefaultTaskFactory : public ITaskFactory
	{
	private:
		unsigned int numberOfTask;
		static void RunDispatcher(void* dsp);

	public:
		// д╛хон╙ number of processor x 2
		DefaultTaskFactory(unsigned int nTask = 0);
		~DefaultTaskFactory(void);

		virtual unsigned int Create(
			ISocketEventDispatcher& dispatcher, 
			SocketServer& server);
	};

}

#endif