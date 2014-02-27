// WhisperServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "WhisperSocketEventHandler.h"
#include "TcpConnectionManager.h"
#include "TcpSocketEventDispatcher.h"
#include "TcpSocketFactory.h"
#include "TcpSocketServer.h"
#include "Business\Handlers\AuthenticateRequestHandler.h"
#include "Business\PacketDispatcher.h"

Framework::WSAInitializor init;

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace Framework;
	using namespace log4cplus;
	using namespace std;
	using namespace TcpSocketServerSide;
	using namespace Business;
	using namespace Business::Handlers;
	
	
	// configure log4cplus
	SharedAppenderPtr appender(new DailyRollingFileAppender(_T("Whisper.log")));
	//tstring patternExp = _T("%-5p - %Y-%m-%d %H:%M:%S - %l - %m%n");
	tstring patternExp = _T("%-5p - %D - %l - %m%n");
	auto_ptr<Layout> layout(new PatternLayout(patternExp));
	appender->setLayout(layout);
	Logger logger = Logger::getRoot();
	logger.addAppender(appender);

	// configure Framework
	Configurations::BufferSize = 1024;
	Configurations::Logger = &logger;

	try{

		ITaskFactory* taskFactory = new DefaultTaskFactory(1);

		SocketServer* server = new TcpSocketServer(
			"127.0.0.1", //"192.168.1.103",//
			8888,
			taskFactory
			);

		auto dispatcher = new TcpSocketEventDispatcher(*server);
		auto socketFactory = new TcpSocketFactory();
		auto manager = new TcpConnectionManager(*server, *dispatcher, *socketFactory);
		auto handler = new WhisperSocketEventHandler(*server);
		auto serverEventHandler = 
			new WhisperSocketEventHandler::ServerEventHandler(
			*server,
			*handler
			);

		/* register message handler */
		PacketDispatcher& packetDispatcher = handler->GetPacketDispatcher();
		packetDispatcher.Register(new AuthenticateRequestHandler());


		/* register message handler */

		server->SetSocketEventDispatcher(dispatcher);
		server->SetConnectionManager(manager);
		server->SetSocketEventHandler(handler);
		server->SetServerEventHandler(serverEventHandler);

		server->Start();

		/*Sleep(10*1000);*/
		//server->Shutdown();

		server->WaitForShutdown();
		delete server;
		cout << " Server closed." << endl;
	}
	catch(std::exception &ex){
		cout << ex.what() << endl;
	}

	cin.get();

	return 0;
}

