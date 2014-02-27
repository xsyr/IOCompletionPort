
#ifndef _PriorityQueue_H_
#define _PriorityQueue_H_

#include <Windows.h>
#include <queue>
#include <Uncopyable.h>
#include <StackOnly.h>

namespace Framework
{

	template<typename T>
	class PriorityQueue : private Uncopyable{
	private:
		CRITICAL_SECTION lock;
		std::queue<T> q;

		PriorityQueue(const PriorityQueue&);
		PriorityQueue& operator=(const PriorityQueue&);

	public:
		PriorityQueue(){
			::InitializeCriticalSection(&this->lock);
		}

		~PriorityQueue(){
			::DeleteCriticalSection(&this->lock);
		}

		CRITICAL_SECTION& GetLock(){
			return this->lock;
		}

		void Enqueue(const T& x){
			this->q.push(x);
		}

		T Dequeue(){
			auto x = this->q.front();
			this->q.pop();
			return x;
		}

		size_t Size() const{
			return this->q.size();
		}

		bool IsEmpty() const{
			return this->q.empty();
		}

		class Owner : StackOnly{
		private:
			CRITICAL_SECTION& lock;

			Owner(const Owner&);
			Owner& operator=(const Owner&);

		public:
			explicit Owner(CRITICAL_SECTION& lock) : lock(lock){
				::EnterCriticalSection(&this->lock);
			}

			~Owner(){
				::LeaveCriticalSection(&this->lock);
			}

		};
	};

}

#endif