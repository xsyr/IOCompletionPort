
#include <stdafx.h>
#include <WSAHelper.h>

namespace Framework
{
	LPFN_ACCEPTEX WSAExtMethods::AcceptEx = NULL;
	LPFN_GETACCEPTEXSOCKADDRS WSAExtMethods::GetAcceptExSockaddrs = NULL;
	LPFN_CONNECTEX WSAExtMethods::ConnectEx = NULL;
	LPFN_DISCONNECTEX WSAExtMethods::DisconnectEx = NULL;
	LPFN_TRANSMITFILE WSAExtMethods::TransmitFile = NULL;
	LPFN_TRANSMITPACKETS WSAExtMethods::TransmitPackets = NULL;
	LPFN_WSARECVMSG WSAExtMethods::WSARecvMsg = NULL;
	LPFN_WSASENDMSG WSAExtMethods::WSASendMsg = NULL;


	bool WSAExtMethods::isInit = false;

	WSAExtMethods::WSAExtMethods(){

		if(isInit) return;
		isInit = true;

		SOCKET temp = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		GUID guidAcceptEx = WSAID_ACCEPTEX, 
			guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS,
			guidConnectEx = WSAID_CONNECTEX,
			guidDisconnectEx = WSAID_DISCONNECTEX,
			guidTransmitFile = WSAID_TRANSMITFILE,
			guidTransmitPackets = WSAID_TRANSMITPACKETS,
			guidWSARecvMsg = WSAID_WSARECVMSG,
			guidWSASendMsg = WSAID_WSASENDMSG;


		DWORD dwBytes;
		WSAIoctl(
			temp,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidAcceptEx,
			sizeof(guidAcceptEx),
			&WSAExtMethods::AcceptEx,
			sizeof(WSAExtMethods::AcceptEx),
			&dwBytes,
			NULL,
			NULL
			);

		WSAIoctl(
			temp,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidGetAcceptExSockaddrs,
			sizeof(guidGetAcceptExSockaddrs),
			&WSAExtMethods::GetAcceptExSockaddrs,
			sizeof(WSAExtMethods::GetAcceptExSockaddrs),
			&dwBytes,
			NULL,
			NULL
			);

		WSAIoctl(
			temp,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidConnectEx,
			sizeof(guidConnectEx),
			&WSAExtMethods::ConnectEx,
			sizeof(WSAExtMethods::ConnectEx),
			&dwBytes,
			NULL,
			NULL
			);

		WSAIoctl(
			temp,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidDisconnectEx,
			sizeof(guidDisconnectEx),
			&WSAExtMethods::DisconnectEx,
			sizeof(WSAExtMethods::DisconnectEx),
			&dwBytes,
			NULL,
			NULL
			);

		WSAIoctl(
			temp,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidTransmitFile,
			sizeof(guidTransmitFile),
			&WSAExtMethods::TransmitFile,
			sizeof(WSAExtMethods::TransmitFile),
			&dwBytes,
			NULL,
			NULL
			);


		WSAIoctl(
			temp,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidTransmitPackets,
			sizeof(guidTransmitPackets),
			&WSAExtMethods::TransmitPackets,
			sizeof(WSAExtMethods::TransmitPackets),
			&dwBytes,
			NULL,
			NULL
			);


		WSAIoctl(
			temp,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidWSARecvMsg,
			sizeof(guidWSARecvMsg),
			&WSAExtMethods::WSARecvMsg,
			sizeof(WSAExtMethods::WSARecvMsg),
			&dwBytes,
			NULL,
			NULL
			);


		WSAIoctl(
			temp,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guidWSASendMsg,
			sizeof(guidWSASendMsg),
			&WSAExtMethods::WSASendMsg,
			sizeof(WSAExtMethods::WSASendMsg),
			&dwBytes,
			NULL,
			NULL
			);

		::closesocket(temp);
	};

	namespace WSAUtility
	{
		template<typename TProtocol>
		SOCKET CreateOverlappedSocket(TProtocol){
			throw std::exception();
		};

		template<>
		SOCKET CreateOverlappedSocket<TCP>(TCP){
			return ::WSASocket(AF_INET, 
				SOCK_STREAM, 
				IPPROTO_TCP, 
				NULL, 0, 
				WSA_FLAG_OVERLAPPED);
		};

		template<>
		SOCKET CreateOverlappedSocket<UDP>(UDP){
			return ::WSASocket(AF_INET, 
				SOCK_DGRAM, 
				IPPROTO_UDP, 
				NULL, 0, 
				WSA_FLAG_OVERLAPPED);
		};

		DWORD FormatMessageToString(LPTSTR *ppBuffer, DWORD dwErrCode){
			return FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS, 
				NULL, dwErrCode, 
				//MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),// Default language
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
				(LPTSTR)ppBuffer, 0, NULL
				);
		}

		DWORD FormatMessageToStringA(LPSTR *ppBuffer, DWORD dwErrCode){
			return FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS, 
				NULL, dwErrCode, 
				//MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),// Default language
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
				(LPSTR)ppBuffer, 0, NULL
				);
		}

		DWORD FormatMessageToStringW(LPWSTR *ppBuffer, DWORD dwErrCode){
			return FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS, 
				NULL, dwErrCode, 
				//MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),// Default language
				MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
				(LPWSTR)ppBuffer, 0, NULL
				);
		}

	}
}