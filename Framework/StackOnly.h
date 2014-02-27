
#ifndef _StackOnly_H_
#define _StackOnly_H_

class StackOnly{
private:
	static void * operator new(size_t size);
	static void operator delete(void* pObj);

};

#endif
