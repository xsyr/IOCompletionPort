

#ifndef _IOCompletionPort_H_
#define _IOCompletionPort_H_


#include <Windows.h>
#include <Uncopyable.h>

namespace Framework
{

	class IOCompletionPort : private Uncopyable
	{
	private:
		HANDLE hIocp;

	public:
		explicit IOCompletionPort(DWORD nConcurrentThreads = 0);

		~IOCompletionPort(){
			::CloseHandle(this->hIocp);
		}


		template<typename TCompletionKey>
		bool AssociateSocket(SOCKET socket, TCompletionKey completionKey){

			static_assert(sizeof(completionKey) == sizeof(ULONG_PTR), 
				"Can not convert completionKey to type ULONG_PTR");

			return this->hIocp ==
				::CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(socket),
				this->hIocp,
				(ULONG_PTR)completionKey,
				0
				);
		}

		template<typename TCompletionKey, typename TOverlapped>
		bool GetQueuedCompletionStatus(
			LPDWORD lpNumberOfBytes,
			TCompletionKey *lpCompletionKey,
			TOverlapped **lpOverlapped,
			DWORD dwMilliseconds
			){
				static_assert(sizeof(lpCompletionKey) == sizeof(PULONG_PTR), 
					"Can not convert lpCompletionKey to type PULONG_PTR");

				static_assert(sizeof(lpOverlapped) == sizeof(LPOVERLAPPED), 
					"Can not convert lpOverlapped to type LPOVERLAPPED");

				return TRUE == ::GetQueuedCompletionStatus(
					this->hIocp,
					lpNumberOfBytes,
					reinterpret_cast<PULONG_PTR>(lpCompletionKey),
					reinterpret_cast<LPOVERLAPPED*>(lpOverlapped),
					dwMilliseconds
					);
		}

		template<typename TCompletionKey, typename TOverlapped>
		bool PostQueuedCompletionStatus(
			DWORD numberOfBytes,
			TCompletionKey completionKey,
			TOverlapped *lpOverlapped
			){
				static_assert(sizeof(completionKey) == sizeof(PULONG_PTR), 
					"Can not convert lpCompletionKey to type PULONG_PTR");

				static_assert(sizeof(lpOverlapped) == sizeof(LPOVERLAPPED), 
					"Can not convert lpOverlapped to type LPOVERLAPPED");

				return TRUE == ::PostQueuedCompletionStatus(
					this->hIocp,
					numberOfBytes,
					(ULONG_PTR)(completionKey),
					static_cast<LPOVERLAPPED>(lpOverlapped)
					);
		}
	};

}

#endif