
#include "stdafx.h"
#include "PacketWrapper.h"
#include <IOBuffer.h>

namespace Business
{
	using namespace google::protobuf;

	PacketWrapper::PacketWrapper(Framework::IOBuffer& buffer)
		: packetSize(0), reserve(0), raw(nullptr), btr(0), currentRead(0),
		isComplete(false), msgType(0, 0)
	{
		this->Reset(buffer.GetPointer(), buffer.GetUsed());
	}

	PacketWrapper::PacketWrapper(Message& message, PacketType& type)
		: packetSize(0), reserve(0), raw(nullptr), btr(0), currentRead(0),
		isComplete(false), msgType(type)
	{
		int size = message.GetCachedSize();
		if(0 == size){
			size = message.ByteSize();
		}

		this->packetSize = size + SIZEOFHEADER;
	}

	bool PacketWrapper::SerializeTo(void* to, unsigned int capacity){
		if(SIZEOFHEADER > capacity){
			return false;
		}

		unsigned int* p = 
			reinterpret_cast<unsigned int*>(to);

		*p = ::htonl(this->packetSize);
		++p;
		*p = ::htonl(this->msgType);
		++p;
		*p = ::htonl(this->reserve);

		return true;
	}

	void PacketWrapper::Reset(const void* buffer, unsigned int length){

		this->raw = buffer;
		this->currentRead = length;
		this->packetSize = 0;
		this->msgType = 0;
		this->reserve = 0;

		if(4 > length){
			this->isComplete = false;
			this->btr = 4 - length;
		}
		else{
			const unsigned int* p = 
				reinterpret_cast<const unsigned int*>(buffer);

			this->packetSize = ::ntohl(*p);
			this->isComplete = length >= this->packetSize;

			if(this->isComplete){
				++p;
				this->msgType = ::ntohl(*p);
				++p;
				this->reserve = ::ntohl(*p);
			}

			this->btr = isComplete ? 0 : this->packetSize - length;
		}

	}

}