
#include "stdafx.h"
#include "Serializor.h"
#include <IOBuffer.h>
#include "PacketWrapper.h"

namespace Business
{
	using namespace std;
	using namespace google::protobuf;
	using namespace Framework;

	bool Serializor::Serialize(
		Message& message,
		PacketWrapper& wrapper,
		IOBuffer& buffer){

			int msgSize = message.GetCachedSize();
			if(0 == msgSize){
				msgSize = message.ByteSize();
			}

			wrapper.SerializeTo(
				buffer.GetPointer(buffer.GetUsed()),
				buffer.GetResidualCapacity()
				);

			buffer.SetUsed(buffer.GetUsed() + PacketWrapper::SIZEOFHEADER);

			message.SerializeToArray(
				buffer.GetPointer(buffer.GetUsed()),
				buffer.GetResidualCapacity()
				);

			buffer.SetUsed(buffer.GetUsed() + msgSize);

			return true;
	}

}