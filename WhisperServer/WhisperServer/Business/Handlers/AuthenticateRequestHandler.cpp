
#include "stdafx.h"
#include "AuthenticateRequestHandler.h"
#include "../PacketWrapper.h"

namespace Business
{
	namespace Handlers
	{

		Messages::Authenticate::Request* AuthenticateRequestHandler::NewMessage(){
			return new Messages::Authenticate::Request();
		}

		PacketType AuthenticateRequestHandler::GetType(){
			return PacketType(MessageType::t_Authenticate, MessageType::code_Auth_Request);
		}

		bool AuthenticateRequestHandler::IsTypeOf(PacketType type){
			return type == this->GetType();
		}

		void AuthenticateRequestHandler::Handle(
			Request<Messages::Authenticate::Request>& request, 
			Response& response)
		{
			unsigned int account = request.GetMessage().account();
			const std::string& pwd = request.GetMessage().password();
			Messages::Authenticate::Response res;
			if(pwd != std::string("123456")){
				res.set_isok(false);
				res.set_description("Password must equal '123456'.");
			}
			else{
				res.set_isok(true);
				res.set_description("Ok");
			}

			PacketWrapper wrapper(
				res, 
				PacketType(
				Business::MessageType::t_Authenticate,
				Business::MessageType::code_Auth_Response)
				);

			response.Write(res, wrapper);
		}

	}
}