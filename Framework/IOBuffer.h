
#ifndef _IOBuffer_H_
#define _IOBuffer_H_


#include <IOOperation.h>
#include <Framework.h>

#include <WinSock2.h>

namespace Framework
{

	class IOBuffer : public WSAOVERLAPPED{
	private:
		unsigned long bufferSize;
		unsigned long used;
		unsigned long attachment;
		unsigned long seek;
		Socket& socket;
		IO_Operation operationType;
		char buffer[0];

		IOBuffer(const IOBuffer&);
		IOBuffer& operator=(const IOBuffer&);

	public:
		IOBuffer(unsigned int size, Socket& skt) : 
			used(0), 
			seek(0),
			socket(skt),
			bufferSize(size),
			attachment(0)
		{
			memset(this, 0, sizeof(WSAOVERLAPPED));
		}

		void Clear(){ this->used = 0; }

		Socket& GetSocket(){ return this->socket; }

		unsigned long GetSize() const{ return this->bufferSize; }

		unsigned long GetUsed() const{ return this->used; }

		unsigned long GetResidualCapacity() const{
			return this->bufferSize - this->used;
		}

		void Seek(unsigned long offset){
			if( this->bufferSize <= offset ){
				throw std::exception("Out of range");
			}

			this->seek = offset;
		}

		unsigned long Seek() const{
			return this->seek;
		}

		template<typename T>
		void SetAttachment(T t){
			static_assert(sizeof(T) == sizeof(unsigned long), 
				"The size of T must equal or less than size of unsigned long");

			this->attachment = reinterpret_cast<unsigned long>(t);
		}

		template<typename T>
		T GetAttachmentAs(){
			return reinterpret_cast<T>(this->attachment);
		}

		void SetUsed(unsigned long size){ this->used = size; }

		IO_Operation GetOperation() const{ return this->operationType; }

		void SetOperation(IO_Operation operation){ this->operationType = operation; }

		// 根据 seek 返回指针
		void* GetPointer();

		// 根据 index 返回指针
		void* GetPointer(unsigned long index);

		void CopyFrom(unsigned long startIndex, const void* source, int length);

		unsigned long Append(const void *data, unsigned long length);
		static void* operator new(size_t objSize, size_t bufferSize);
		static void operator delete(void *pObj);
		static void operator delete(void *pObj, size_t);


	};

}

#endif