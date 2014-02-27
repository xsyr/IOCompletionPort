
#include "stdafx.h"
#include "Deserializor.h"
#include "PacketWrapper.h"
#include "PacketDispatcher.h"
#include "IMessageHandler.h"

namespace Business
{
	using namespace std;
	using namespace google::protobuf;
	using namespace Framework;

	bool Deserializor::Deserialize(
		Socket& socket, 
		IOBuffer& buffer,
		PacketWrapper& wrapper,
		Message*& message,
		IMessageHandler<Message>*& handler)
	{
		message = nullptr;
		handler = nullptr;

		if(!this->ParsePacket(socket, buffer, wrapper)){
			return false;
		}

		IMessageHandler<Message>* hdl = this->dispatcher.FindHandler(wrapper.GetType());
		handler = hdl;
		message = handler->NewMessage();
		message->ParseFromArray(wrapper.GetContent(), wrapper.GetContentSize());

		return true;
	}

	bool Deserializor::ParsePacket(
		Socket& socket, 
		IOBuffer& buffer,
		PacketWrapper& wrapper)
	{
		unsigned int bytesToRead;
		bool isComplete = wrapper.IsComplete(bytesToRead);

		if(!isComplete){

			if(buffer.GetResidualCapacity() > bytesToRead){
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
}