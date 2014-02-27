
#ifndef PACKETPARSER_H_INCLUDED
#define PACKETPARSER_H_INCLUDED

#include "Business.h"
#include <google\protobuf\message.h>
#include <Framework.h>

namespace Business
{
	class PacketWrapper{
	private:
		/* packet header */
		unsigned int packetSize;
		PacketType msgType;
		unsigned int reserve;
		/* packet header */

		unsigned int currentRead;
		const void * raw;
		bool isComplete;
		unsigned int btr;	// bytes to read

	public:
		static const unsigned int SIZEOFHEADER 
			= 2 * sizeof(unsigned int) + sizeof(PacketType);

	public:
		explicit
			PacketWrapper(Framework::IOBuffer& buffer);

		explicit
			PacketWrapper(google::protobuf::Message& message, PacketType& type);

		bool SerializeTo(void* to, unsigned int capacity);


		PacketType GetType() const{
			return this->msgType;
		}

		void SetType(PacketType type){
			this->msgType = type;
		}


		unsigned int GetCurrentRead() const{
			return this->currentRead;
		}


		unsigned int GetPacketSize() const{
			return this->packetSize;
		}

		unsigned int GetReserve() const{
			return this->reserve;
		}

		void SetReserve(unsigned int data){
			this->reserve = data;
		}

		const void* GetRaw() const{
			return this->raw;
		}

		const void* GetContent() const{
			if(this->isComplete){
				return reinterpret_cast<const char*>(this->raw) + SIZEOFHEADER;
			}
			else{
				return nullptr;
			}
		}

		unsigned int GetContentSize() const{
			return this->packetSize == 0 ? 0 : this->packetSize - SIZEOFHEADER;
		}


		bool IsComplete(unsigned int& bytesToRead) const{
			bytesToRead = this->btr;
			return this->isComplete;
		}

		void Reset(const void* buffer, unsigned int length);
	};

}

#endif