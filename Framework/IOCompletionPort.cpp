
#include "stdafx.h"
#include <IOCompletionPort.h>

namespace Framework
{

	IOCompletionPort::IOCompletionPort(DWORD nConcurrentThreads){
		using std::exception;

		this->hIocp = CreateIoCompletionPort(
			INVALID_HANDLE_VALUE,
			NULL,
			NULL,
			nConcurrentThreads
			);

		if(NULL == this->hIocp){
			throw exception("CreateIoCompletionPort(...) failed");
		}
	}

/*
	template<typename TCompletionKey>
	bool IOCompletionPort::AssociateSocket(SOCKET socket, TCompletionKey completionKey){

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
		bool IOCompletionPort::GetQueuedCompletionStatus(
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
		bool IOCompletionPort::PostQueuedCompletionStatus(
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
					reinterpret_cast<ULONG_PTR>(completionKey),
					static_cast<LPOVERLAPPED>(lpOverlapped)
					);
		}
*/
}