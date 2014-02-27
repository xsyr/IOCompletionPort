
#ifndef DESERIALIZOR_H_INCLUDED
#define DESERIALIZOR_H_INCLUDED

#include <google\protobuf\message.h>
#include "Business.h"
#include <Framework.h>

namespace Business
{
	class Deserializor{
	private:
		PacketDispatcher& dispatcher;

	public:
		Deserializor(PacketDispatcher& dsp) : 
			dispatcher(dsp){

		}

		bool ParsePacket(
			Framework::Socket& socket, 
			Framework::IOBuffer& buffer,
			PacketWrapper& wrapper);

		bool Deserialize(
			Framework::Socket& socket, 
			Framework::IOBuffer& buffer,
			PacketWrapper& wrapper,
			google::protobuf::Message*& message,
			IMessageHandler<google::protobuf::Message>*& handler
			);

	};
}


#endif