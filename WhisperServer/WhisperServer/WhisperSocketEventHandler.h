

#ifndef _WhisperSocketEventHandler_H_
#define _WhisperSocketEventHandler_H_

#include <ISocketEvent.h>
#include <IServerEvent.h>
#include <SocketServer.h>
#include <Socket.h>
#include <map>
#include "Business\PacketDispatcher.h"

class WhisperSocketEventHandler :
	public Framework::ISocketEvent
{
private:
	std::map<SOCKET, Framework::Socket*> socketSet;
	Framework::SocketServer& server;
	Business::PacketDispatcher packetDispatcher;

public:

	class ServerEventHandler : public Framework::IServerEvent{
	private:
		Framework::SocketServer& server;
		WhisperSocketEventHandler& eventHandler;

	public:
		ServerEventHandler(
			Framework::SocketServer& svr,
			WhisperSocketEventHandler& handler)
			:
			server(svr),
			eventHandler(handler){

		}

		void OnError(){};
		void PostShutdown();
		void PostStart(){};
		void PreShutdown();
		void PreStart(){};
	};

	WhisperSocketEventHandler(Framework::SocketServer& svr);

	~WhisperSocketEventHandler(void);


	Business::PacketDispatcher& GetPacketDispatcher(){
		return this->packetDispatcher;
	}


	virtual void PreAccept(Framework::Socket& socket, Framework::IOBuffer& buffer);
	virtual void PostAccept(Framework::Socket& socket, Framework::IOBuffer& buffer);
	virtual void OnError(Framework::Socket& socket){};
	virtual void PostClose(Framework::Socket& socket);
	virtual void PostRead(Framework::Socket& socket, Framework::IOBuffer& buffer);
	virtual void PostWrite(Framework::Socket& socket, Framework::IOBuffer& buffer);
	virtual void PreClose(Framework::Socket& socket){};
	virtual void PreRead(Framework::Socket& socket, Framework::IOBuffer& buffer){};
	virtual void PreWrite(Framework::Socket& socket, Framework::IOBuffer& buffer){};

};


#endif
