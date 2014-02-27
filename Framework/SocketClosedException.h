
#ifndef _SocketClosedException_H_
#define _SocketClosedException_H_

#include <exception>

namespace Framework
{

	class SocketClosedException :
		public std::exception
	{
	public:
		SocketClosedException(const char *message = "The socket is closed.")
		{

		}
	};

}

#endif