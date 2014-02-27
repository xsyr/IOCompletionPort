
#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <WinSock2.h>
#include <MSWSock.h>
#include <exception>
#include <vector>

#include <Framework.h>
#include <IOBuffer.h>
#include <IOCompletionPort.h>
#include <ISocketEvent.h>
#include <PriorityQueue.h>
#include <Uncopyable.h>
#include <StackOnly.h>


namespace Framework
{
	class Socket : private Uncopyable{
	private:
		enum SocketStatus
		{
			DenyReceive = SD_RECEIVE,
			DenySend = SD_SEND,
			DenySendnReceive = SD_BOTH,
			Closed = DenySendnReceive + 1,
			Normal = Closed + 1
		};

		static char* StatusDescription[];

		bool DoWrite(IOBuffer* buffer);


	protected:
		SocketStatus status;
		sockaddr_in peer;
		SOCKET socket;
		SocketServer& server;
		IOCompletionPort* iocp;
		ISocketEventDispatcher& dispatcher;
		PriorityQueue<IOBuffer*> pendingWrites;
		long pendingReads;
		long reference;
		long isClosed;

		typedef PriorityQueue<IOBuffer*> QueueLock;

		void Init(SOCKET socket, SocketServer& svr);

		class SocketEventHandler : public ISocketEvent{
		public:
			virtual void PreAccept(Socket& socket, IOBuffer& buffer){}
			virtual void PostAccept(Socket& socket, IOBuffer& buffer){}
			virtual void OnError(Socket& socket){}
			virtual void PostClose(Socket& socket);
			virtual void PostRead(Socket& socket, IOBuffer& buffer);
			virtual void PostWrite(Socket& socket, IOBuffer& buffer);
			virtual void PreClose(Socket& socket){}
			virtual void PreRead(Socket& socket, IOBuffer& buffer);
			virtual void PreWrite(Socket& socket, IOBuffer& buffer);
		};

	public:
		Socket(
			SOCKET socket, 
			SocketServer& svr, 
			ISocketEventDispatcher& dispatcher, 
			const sockaddr_in& addr);

		Socket(
			SOCKET socket, 
			SocketServer& svr,
			ISocketEventDispatcher& dispatcher);

		class Holder : StackOnly{
		private:
			Socket& skt;

		public:
			Holder(Socket& s) : skt(s){
				s.IncreaseRef();
			}

			~Holder(){
				this->skt.DecreaseRef();
			}
		};

		SOCKET AsSocket() const{
			return this->socket;
		}

		virtual ~Socket();

		void Close();

		void OnReadZeroByte(IOBuffer* buffer);

		sockaddr_in* GetPeerAddress(){
			return &this->peer;
		}

		SocketServer& GetServer(){
			return this->server;
		}

		long IncreaseRef(){
			return ::InterlockedIncrement(&this->pendingReads);
		}

		long DecreaseRef(){
			long ref = ::InterlockedDecrement(&this->reference);
			if(0 == ref){
				delete this;
			}

			return ref;
		}

		bool IsConnected(unsigned long& time);

		bool Read(IOBuffer* buffer);
		void Shutdown(int how);
		void Write(IOBuffer* buffer);
		void Write(const std::vector<IOBuffer*>& buffers);

		static void RegisterEventHandler(ISocketEventDispatcher& dispatcher);
	};

}



#endif