#include "stdafx.h"
#include "WhisperSocketEventHandler.h"
#include <Configurations.h>
#include <SocketServer.h>

using Framework::IOBuffer;
using Framework::Socket;
using Framework::Configurations;
using Framework::SocketServer;

using namespace std;

WhisperSocketEventHandler
	::WhisperSocketEventHandler(SocketServer& svr)
	: server(svr){

}


WhisperSocketEventHandler
	::~WhisperSocketEventHandler(void)
{

}


void WhisperSocketEventHandler
	::ServerEventHandler
	::PreShutdown(){
	

}

void WhisperSocketEventHandler
	::ServerEventHandler
	::PostShutdown(){
	auto first = this->eventHandler.socketSet.begin();
	auto end = this->eventHandler.socketSet.end();

	for(; first != end; ++first){
		first->second->Close();
	}

	this->eventHandler.socketSet.clear();
}

void WhisperSocketEventHandler::PostAccept(
	Framework::Socket& socket, 
	Framework::IOBuffer& buffer){
		cout << " Socket accepted: " << socket.AsSocket() << endl;
		this->packetDispatcher.Dispatch(socket, buffer);

		/*IOBuffer* readBuf = 
			new (Configurations::BufferSize) 
			IOBuffer(Configurations::BufferSize, socket);

		socket.Read(readBuf);*/
}

void WhisperSocketEventHandler::PostRead(
	Framework::Socket& socket, 
	Framework::IOBuffer& buffer){

		this->packetDispatcher.Dispatch(socket, buffer);

		/*IOBuffer* readBuf = 
			new (Configurations::BufferSize) 
			IOBuffer(Configurations::BufferSize, socket);

		socket.Read(readBuf);*/

}

void WhisperSocketEventHandler::PostClose(
	Framework::Socket& socket){
		cout << " Socket   closed: " << socket.AsSocket() << endl;
		if(!this->server.IsShutting()){
			this->socketSet.erase(socket.AsSocket());
		}
};

void WhisperSocketEventHandler::PostWrite(
	Framework::Socket& socket, 
	Framework::IOBuffer& buffer){
		delete &buffer;
}

void WhisperSocketEventHandler::PreAccept(
	Framework::Socket& socket, 
	Framework::IOBuffer& buffer){
		std::cout << " Waiting for connection request" << std::endl;

}