#include "stdafx.h"
#include "Statistics.h"


namespace Framework
{

	// 当前正在运行的工作线程数
	unsigned long Statistics::AliveTask = 0;
	// 当前未完成的写操作
	unsigned long Statistics::PendingWrites = 0;
	// 当前未完成的读操作
	unsigned long Statistics::PendingReads = 0;
	// 当前已连接的 socket 数
	unsigned long Statistics::ConnectingSockets = 0;
}