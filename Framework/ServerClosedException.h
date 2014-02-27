

#ifndef _ServerClosedException_H_
#define _ServerClosedException_H_

#include <exception>

namespace Framework
{

	class ServerClosedException :
		public std::exception
	{
	public:
		ServerClosedException(const char *message = "The server is closed.")
		{

		}
	};

}

#endif