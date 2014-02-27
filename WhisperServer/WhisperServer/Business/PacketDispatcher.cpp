
#include "stdafx.h"
#include "PacketDispatcher.h"
#include "IMessageHandler.h"
#include "PacketWrapper.h"
#include "Deserializor.h"


namespace Business
{

	using namespace std;
	using namespace Framework;
	using namespace google::protobuf;

	PacketDispatcher::PacketDispatcher()
		: isRunning(false), deserializor(*this){

	}

	PacketDispatcher::~PacketDispatcher(){
		auto iter = handlers.begin();
		for(; iter != handlers.end(); ++iter){
			try{
				delete *iter;
			}
			catch(...){}
		}
	}

	bool PacketDispatcher::ParsePacket(
		Socket& socket,
		IOBuffer& buffer,
		PacketWrapper& wrapper)
	{

		unsigned int bytesToRead;
		bool isComplete = wrapper.IsComplete(bytesToRead);

		if(!isComplete){

			if(0 == wrapper.GetCurrentRead()){
				buffer.SetUsed(0);
				buffer.Seek(0);
				socket.Read(&buffer);
			}
			else if(buffer.GetResidualCapacity() > bytesToRead){
				socket.Read(&buffer);
			}
			else{
				if(bytesToRead < buffer.Seek() - 1 + buffer.GetResidualCapacity()){

					buffer.CopyFrom(0, wrapper.GetRaw(), wrapper.GetCurrentRead());
					buffer.SetUsed(wrapper.GetCurrentRead());
					buffer.Seek(0);
					socket.Read(&buffer);
				}
				else{
					IOBuffer* buf = 
						new(wrapper.GetPacketSize() + 10) 
						IOBuffer(wrapper.GetPacketSize() + 10, socket);

					buf->CopyFrom(0, wrapper.GetRaw(), wrapper.GetCurrentRead());
					buf->SetUsed(wrapper.GetCurrentRead());
					buf->Seek(0);
					socket.Read(buf);

					delete &buffer;
				}


			}
		}

		return isComplete;
	}

	IMessageHandler<Message>* PacketDispatcher::FindHandler(PacketType type)
	{

		struct handler_predicate
		{
			PacketType type;

			explicit
				handler_predicate(PacketType t)
				:type(t){
			}

			bool operator()(IMessageHandler<Message>* handler){
				return handler->IsTypeOf(this->type);
			}

		};

		auto iter = find_if(
			this->handlers.begin(), 
			this->handlers.end(), 
			handler_predicate(type)
			);

		return *iter;
	}


	void PacketDispatcher::Dispatch(
		Socket& socket, 
		IOBuffer& buffer)
	{

		PacketWrapper wrapper(buffer);

		while (true)
		{
			if(!this->ParsePacket(socket, buffer, wrapper)){
				break;
			}

			Message* message = nullptr;
			IMessageHandler<Message>* handler = nullptr;

			if(!this->deserializor.Deserialize(socket, buffer,
				wrapper, message, handler))
			{
				break;
			}

			auto_ptr<Message> msg(message);
			message = nullptr;

			Response response(this->serializor, socket);
			Request<Message> request(socket, buffer, *msg);

			try{
				handler->Handle(request, response);
			}
			catch(...){

			}

			response.Flush();

			buffer.Seek(wrapper.GetPacketSize());
			wrapper.Reset(buffer.GetPointer(), buffer.GetUsed() - buffer.Seek());
		}

	}

};