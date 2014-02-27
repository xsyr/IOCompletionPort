
#include "stdafx.h"
#include "Response.h"
#include "PacketWrapper.h"
#include <IOBuffer.h>
#include "../Config.h"
#include "Serializor.h"


namespace Business
{
	using namespace std;
	using namespace Framework;
	using namespace google::protobuf;
	using namespace Whisper::Config;

	Response::Response(Serializor& slr, Socket& skt)
		: serializor(slr), socket(skt)
	{

	}

	void Response::Flush(){
		if(this->buffers.size() > 0){
			vector<IOBuffer*> sending(this->buffers);
			this->buffers.clear();
			this->socket.Write(sending);
		}
	}

	void Response::Write(Message& message, PacketWrapper& wrapper){

		
		int msgSize = message.ByteSize();
		int totalSize = msgSize + PacketWrapper::SIZEOFHEADER;

		IOBuffer* buffer = nullptr;

		if(this->buffers.size() > 0){
			buffer = this->buffers.back();
		}

		if(nullptr == buffer
			|| buffer->GetResidualCapacity() < totalSize)
		{
			if(IOBUFFERSIZE > totalSize){
				buffer = 
					new(IOBUFFERSIZE)
					IOBuffer(IOBUFFERSIZE, this->socket);
			}
			else{
				buffer = 
					new(totalSize)
					IOBuffer(totalSize, this->socket);
			}

			this->buffers.push_back(buffer);
		}

		this->serializor.Serialize(message, wrapper, *buffer);
	}

}