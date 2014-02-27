
#ifndef RESPONSE_H_INCLUDED
#define RESPONSE_H_INCLUDED

#include <Framework.h>
#include <google\protobuf\message.h>
#include "Business.h"

namespace Business
{
	class Response : private Uncopyable{
	private:
		Serializor& serializor;
		Framework::Socket& socket;
		std::vector<Framework::IOBuffer*> buffers;

	public:
		Response(Serializor& slr, Framework::Socket& skt);

		void Flush();

		Framework::Socket& GetSocket(){
			return this->socket;
		}

		void Write(google::protobuf::Message& message, PacketWrapper& wrapper);

	};

};

#endif