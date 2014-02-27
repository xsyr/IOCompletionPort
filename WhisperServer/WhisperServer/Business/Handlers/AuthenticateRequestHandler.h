
#ifndef AUTHENTICATEREQUEST_H_INCLUDED
#define AUTHENTICATEREQUEST_H_INCLUDED

#include "../IMessageHandler.h"
#include <Messages\MessageDef.h>
#include <Messages\Authenticate.pb.h>

namespace Business
{
	namespace Handlers
	{
		class AuthenticateRequestHandler : public IMessageHandler<Messages::Authenticate::Request>
		{
		public:
			virtual Messages::Authenticate::Request* NewMessage();
			virtual PacketType GetType();
			virtual void Handle(Request<Messages::Authenticate::Request>& request, Response& response);
			virtual	bool IsTypeOf(PacketType type);

		};

	}

}

#endif