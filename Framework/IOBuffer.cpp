
#include "stdafx.h"
#include <IOBuffer.h>

namespace Framework
{

	unsigned long IOBuffer::Append(const void *data, unsigned long length){
		if(nullptr == data){
			return 0;
		}

		unsigned long validCapacity = this->GetSize() - this->GetUsed();

		if(0 == validCapacity){
			return 0;
		}

		unsigned long appended = validCapacity > length ? length : validCapacity;


		memcpy(
			&this->buffer[this->GetUsed()], 
			data, 
			appended
			);

		this->used += appended;

		return appended;
	}

	void* IOBuffer::GetPointer(){
		return &this->buffer[seek];
	}

	void* IOBuffer::GetPointer(unsigned long index){
			if(index >= this->GetSize()){
				std::exception("Out of range");
			}

			return &this->buffer[index];
		}

	void IOBuffer::CopyFrom(unsigned long startIndex, const void* source, int length){
		::memcpy(this->GetPointer(startIndex), source, length);
	}

	void* IOBuffer::operator new(size_t objSize, size_t bufferSize){
		return ::operator new(objSize + bufferSize);
	}


	void IOBuffer::operator delete(void *pObj){
		if(nullptr != pObj){
			delete[] static_cast<char*>(pObj);
		}
	}

	void IOBuffer::operator delete(void *pObj, size_t){
		if(nullptr != pObj){
			delete[] static_cast<char*>(pObj);
		}
	}

}