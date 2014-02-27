
#include "stdafx.h"
#include <SocketServer.h>
#include <WSAHelper.h>
#include <Macros.h>
#include <IConnectionManager.h>
#include <ITaskFactory.h>
#include <IOCompletionPort.h>
#include <ISocketEventDispatcher.h>
#include <IServerEvent.h>
#include <Socket.h>
#include <Statistics.h>

namespace Framework
{
	SocketServer::SocketServer(
		ITaskFactory* factory, 
		DWORD nConcurrentThreads /* = 0 */ )
		: eventShutdown(0),
		isStartup(false),
		isShutting(false),
		numberOfTask(0),
		iocp(nConcurrentThreads),
		taskFactory(factory),
		serverEventHandler(nullptr),
		socketEventHandler(nullptr),
		socketEventDispatcher(nullptr),
		monitor(nullptr)
	{
		this->eventShutdown = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	}


	SocketServer::~SocketServer(void)
	{
		::CloseHandle(this->eventShutdown);

		delete this->taskFactory;
		this->taskFactory = nullptr;

		delete this->serverEventHandler;
		this->serverEventHandler = nullptr;

		//delete this->socketEventHandler;


		delete this->socketEventDispatcher;

		delete this->monitor;
		this->monitor = nullptr;
	}

	void SocketServer::Shutdown(){

		if(nullptr != this->serverEventHandler){
			this->serverEventHandler->PreShutdown();
		}

		this->isShutting = true;
		Socket* mock = nullptr;
		std::auto_ptr<IOBuffer> mockBuf(new(1) IOBuffer(1, *mock));

		mockBuf->SetOperation(IO_Termination);

		for (unsigned int i = 0; i < this->numberOfTask; i++)
		{
			this->iocp.PostQueuedCompletionStatus(0, 0, mockBuf.get());
		}


		// wait to complete
		HANDLE timer = ::CreateEvent(NULL,  TRUE, FALSE, NULL);
		while (true)
		{
			::WaitForSingleObject(timer, 500);
			// when to break
			if(0 == ::InterlockedCompareExchange(
				&Statistics::AliveTask, 
				Statistics::AliveTask, 
				Statistics::AliveTask))
			{
				break;
			}
		}

		::CloseHandle(timer);

		if(nullptr != this->serverEventHandler){
			this->serverEventHandler->PostShutdown();
		}

		::SetEvent(this->eventShutdown);
	}

	void SocketServer::Start(){
		if(nullptr == this->taskFactory){
			throw std::exception("Pleae set up the task factory.");
		}

		if(nullptr == this->socketEventHandler){
			throw std::exception("Please set up the socket event handler.");
		}

		if(nullptr == this->socketEventDispatcher){
			throw std::exception("Please set up the socket event dispathcher.");
		}

		if(nullptr == this->monitor){
			throw std::exception("Please set up the connection monitor.");
		}

		this->OnStartup();

		if(nullptr != this->serverEventHandler){
			this->serverEventHandler->PreStart();
		}

		this->socketEventDispatcher->Register(this->socketEventHandler);
		Socket::RegisterEventHandler(*this->socketEventDispatcher);
		Statistics::AliveTask = 
			this->numberOfTask =
			this->taskFactory->Create(*this->socketEventDispatcher, *this);

		this->monitor->Process(*this);

		this->isStartup = true;

		if(nullptr != this->serverEventHandler){
			this->serverEventHandler->PostStart();
		}
	}

	void SocketServer::WaitForShutdown(){

		::WaitForSingleObject(this->eventShutdown, INFINITE);
	}
}