

#ifndef _Statistics_H_
#define _Statistics_H_

namespace Framework
{
	class Statistics
	{
	public:
		// 当前正在运行的工作线程数
		static unsigned long AliveTask;
		// 当前未完成的写操作
		static unsigned long PendingWrites;
		// 当前未完成的读操作
		static unsigned long PendingReads;
		// 当前已连接的 socket 数
		static unsigned long ConnectingSockets;
	};

}


#endif