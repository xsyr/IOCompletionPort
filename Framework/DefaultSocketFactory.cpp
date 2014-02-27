#include "stdafx.h"
#include "DefaultSocketFactory.h"
#include <WSAHelper.h>
#include <Socket.h>

namespace Framework
{
	Socket* DefaultSocketFactory::Create( 
		SocketServer& server, 
		ISocketEventDispatcher& dispatcher){
			SOCKET sTcp = WSAUtility::CreateOverlappedSocket(WSAUtility::TCP());
			return new Socket(sTcp, server, dispatcher);
	}
}