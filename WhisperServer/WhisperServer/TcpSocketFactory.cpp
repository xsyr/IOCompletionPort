#include "stdafx.h"
#include "TcpSocketFactory.h"
#include <WSAHelper.h>
#include <Socket.h>

namespace TcpSocketServerSide
{
	using namespace Framework;

	Socket* TcpSocketFactory::Create( 
		SocketServer& server, 
		ISocketEventDispatcher& dispatcher){
			SOCKET sTcp = WSAUtility::CreateOverlappedSocket(WSAUtility::TCP());
			return new Socket(sTcp, server, dispatcher);
	}
}