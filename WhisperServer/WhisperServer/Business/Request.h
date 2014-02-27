
#ifndef REQUEST_H_INCLUDED
#define REQUEST_H_INCLUDED

#include <Framework.h>
#include <google\protobuf\message.h>
#include "Business.h"

namespace Business
{
	template<typename TMessage>
	class Request : private Uncopyable{
	private:
		Framework::Socket& socket;
		Framework::IOBuffer& buffer;
		google::protobuf::Message& message;

	public:
		Request(Framework::Socket& skt,
			Framework::IOBuffer& buf,
			google::protobuf::Message& msg)
			: socket(skt), buffer(buf), message(msg){

		}

		TMessage& GetMessage(){
			return static_cast<TMessage&>(this->message);
		}

		Framework::Socket& GetSocket(){
			return this->socket;
		}

		Framework::IOBuffer& GetIOBuffer(){
			return this->buffer;
		}

	};

}

#endif