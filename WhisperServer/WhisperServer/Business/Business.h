
#ifndef BUSINESS_H_INCLUDED
#define BUSINESS_H_INCLUDED

namespace Business
{
	class PacketDispatcher;
	class Deserializor;
	class Serializor;
	class Response;
	template<typename TMessage>
	class Request;

	template<typename TMessage>
	class IMessageHandler;

	class PacketWrapper;


	struct PacketType
	{
		unsigned short type;
		unsigned short code;

		explicit
			PacketType(unsigned short t, unsigned short c)
			: type(t), code(c){
		}

		operator unsigned int(){
			return *reinterpret_cast<const unsigned int*>(this);
		}

		PacketType& operator=(unsigned int t){
			unsigned int* tthis =
				reinterpret_cast<unsigned int*>(
				const_cast<PacketType*>(this)
				);

			*tthis = t;

			return *this;
		}
	};

};

#endif