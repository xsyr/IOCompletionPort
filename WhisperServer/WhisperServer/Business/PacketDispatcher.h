
#ifndef _PacketDispatcher_H_
#define _PacketDispatcher_H_

#include <vector>
#include <google\protobuf\message.h>
#include <Framework.h>
#include "Business.h"
#include "Serializor.h"
#include "Deserializor.h"

namespace Business
{
	class PacketDispatcher : Uncopyable{
	private:
		bool isRunning;
		std::vector<IMessageHandler<google::protobuf::Message>*> handlers;
		Serializor serializor;
		Deserializor deserializor;

		bool PacketDispatcher::ParsePacket(
			Framework::Socket& socket,
			Framework::IOBuffer& buffer,
			PacketWrapper& wrapper);

	public:
		PacketDispatcher();
		~PacketDispatcher();

		void Dispatch(Framework::Socket& socket, Framework::IOBuffer& buffer);

		IMessageHandler<google::protobuf::Message>*
			FindHandler(PacketType type);

		Serializor& GetSerializor(){
			return	this->serializor;
		}

		template<typename TMessage>
		size_t Register(IMessageHandler<TMessage>* handler){
			class TCheck : public TMessage{
			public:
				TCheck(){
					static_cast<google::protobuf::Message*>(this);
				};
			};


			this->handlers.push_back(
				reinterpret_cast<IMessageHandler<google::protobuf::Message>*>(handler)
				);

			return this->handlers.size();
		}
	};

};

#endif