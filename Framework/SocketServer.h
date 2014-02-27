
#ifndef _SocketServer_H_
#define _SocketServer_H_


#include <Framework.h>
#include <Uncopyable.h>
#include <IOCompletionPort.h>
#include <WinSock2.h>
#include <exception>

namespace Framework
{

	class SocketServer : private Uncopyable
	{
	protected:
		HANDLE eventShutdown;
		bool isStartup;
		bool isShutting;
		unsigned int numberOfTask;
		IOCompletionPort iocp;
		ITaskFactory* taskFactory;
		IServerEvent* serverEventHandler;
		ISocketEvent* socketEventHandler;
		ISocketEventDispatcher* socketEventDispatcher;
		IConnectionManager* monitor;

		void Run();
		static unsigned int _stdcall ThreadEntry(void* p){
			SocketServer* server = 
				reinterpret_cast<SocketServer*>(p);
			server->Run();
		}

		virtual void OnStartup() = 0;

	public:
		SocketServer(
			ITaskFactory* factory, 
			DWORD nConcurrentThreads);

		virtual ~SocketServer(void);

		IOCompletionPort& GetIOCompletionPort(){
			return this->iocp;
		}

		IConnectionManager& GetConnectionManager(){
			return *this->monitor;
		}

		bool IsShutting() const{
			return this->isShutting;
		}

		void SetConnectionManager(IConnectionManager* monitor){

			if(nullptr == monitor){
				throw std::exception(
					"The argument 'handler' can not be null"
					);
			}

			if(this->isStartup){
				throw std::exception(
					"Please call SetConnectionManager(...)"
					" beforce the socket server started."
					);
			}

			this->monitor = monitor;
		}

		void SetServerEventHandler(IServerEvent* handler){

			if(nullptr == handler){
				throw std::exception(
					"The argument 'handler' can not be null"
					);
			}

			if(this->isStartup){
				throw std::exception(
					"Please call SetServerEventHandler(...)"
					" beforce the socket server started."
					);
			}

			this->serverEventHandler = handler;
		}

		void SetSocketEventHandler(ISocketEvent* handler){
			if(nullptr == handler){
				throw std::exception(
					"The argument 'handler' can not be null"
					);
			}

			if(this->isStartup){
				throw std::exception(
					"Please call SetSocketEventHandler(...)"
					" beforce the socket server started."
					);
			}

			this->socketEventHandler = handler;
		}

		void SetSocketEventDispatcher(ISocketEventDispatcher* dispatcher){
			if(nullptr == dispatcher){
				throw std::exception(
					"The argument 'dispatcher' can not be null"
					);
			}

			if(this->isStartup){
				throw std::exception(
					"Please call SetSocketEventDispatcher(...)"
					" beforce the socket server started."
					);
			}

			this->socketEventDispatcher = dispatcher;
		}
		
		virtual void Shutdown();
		virtual void Start();
		virtual void WaitForShutdown();

	};

}

#endif
