
#ifndef _WSAHelper_H_
#define _WSAHelper_H_

#include <WinSock2.h>
#include <IPHlpApi.h>
#include <MSWSock.h>

namespace Framework
{

	class WSAExtMethods{
	private:
		static bool isInit;
	public:
		static LPFN_ACCEPTEX AcceptEx;
		static LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;
		static LPFN_CONNECTEX ConnectEx;
		static LPFN_DISCONNECTEX DisconnectEx;
		static LPFN_TRANSMITFILE TransmitFile;
		static LPFN_TRANSMITPACKETS TransmitPackets;
		static LPFN_WSARECVMSG WSARecvMsg;
		static LPFN_WSASENDMSG WSASendMsg;

		WSAExtMethods();
	};


	class WSAInitializor
	{
	private:
		int err;

	public:
		WSAInitializor(){
			WORD wVersion = MAKEWORD(2, 2);
			WSADATA wsData;
			this->err = WSAStartup(wVersion, &wsData);
			if(this->err){
				throw _T("wsa init failed.");
			}

			WSAExtMethods em;
		}

		~WSAInitializor(){
			if(!this->err){
				WSACleanup();
			}
		}
	};



	namespace WSAUtility
	{
		struct TCP{};
		struct UDP{};

		template<typename TProtocol>
		SOCKET CreateOverlappedSocket(TProtocol);

		template<>
		SOCKET CreateOverlappedSocket<TCP>(TCP);

		template<>
		SOCKET CreateOverlappedSocket<UDP>(UDP);

		DWORD FormatMessageToString(LPTSTR *ppBuffer, DWORD dwErrCode);

		DWORD FormatMessageToStringA(LPSTR *ppBuffer, DWORD dwErrCode);

		DWORD FormatMessageToStringW(LPWSTR *ppBuffer, DWORD dwErrCode);

	};
}

#endif