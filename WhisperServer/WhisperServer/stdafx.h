// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <exception>
#include <iostream>
#include <memory>


// TODO: reference additional headers your program requires here

#include <Winsock2.h>
#include <Windows.h>

// log4cplus headers
#include <log4cplus\logger.h>
#include <log4cplus\loggingmacros.h>
#include <log4cplus\appender.h>
#include <log4cplus\fileappender.h>
#include <log4cplus\layout.h>

// framework headers
#include <SocketServer.h>
#include <DefaultTaskFactory.h>
#include <IOBuffer.h>
#include <WSAHelper.h>
#include <Configurations.h>
#include <Statistics.h>
#include <Socket.h>

// fastformat
#include <fastformat\fastformat.hpp>