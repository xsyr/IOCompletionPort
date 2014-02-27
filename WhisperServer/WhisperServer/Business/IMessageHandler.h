
#ifndef IMESSAGEHANDLER_H_INCLUDED
#define IMESSAGEHANDLER_H_INCLUDED

#include "Business.h"
#include "Request.h"
#include "Response.h"

namespace Business
{
	template<typename TMessage>
	class IMessageHandler{
	public:
		virtual TMessage* NewMessage() = 0;
		virtual PacketType GetType() = 0;
		virtual void Handle(Request<TMessage>& request, Response& response) = 0;
		virtual	bool IsTypeOf(PacketType type) = 0;

	};
}

#endif