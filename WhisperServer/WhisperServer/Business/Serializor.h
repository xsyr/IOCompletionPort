
#ifndef SERIALIZOR_H_INCLUDED
#define SERIALIZOR_H_INCLUDED

#include "Business.h"
#include <Framework.h>
#include <google\protobuf\message.h>

namespace Business
{
	class Serializor{
	public:
		bool Serialize(
			google::protobuf::Message& message,
			PacketWrapper& wrapper,
			Framework::IOBuffer& buffer
			);

	};

};

#endif