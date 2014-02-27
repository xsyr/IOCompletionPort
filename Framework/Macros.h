
#ifndef _Macros_H_
#define _Macros_H_

#include <WSAHelper.h>

#define ThrowWSAErrorMessage() \
	do\
	{\
		LPSTR msg = nullptr;\
		WSAUtility::FormatMessageToStringA(&msg, WSAGetLastError());\
		auto ex = std::exception(msg);\
		LocalFree(msg);\
		throw ex;\
	}while(0)

#define ThrowNTErrorMessage() \
	do\
	{\
		LPSTR msg = nullptr;\
		WSAUtility::FormatMessageToStringA(&msg, GetLastError());\
		auto ex = std::exception(msg);\
		LocalFree(msg);\
		throw ex;\
	}while(0)

#endif