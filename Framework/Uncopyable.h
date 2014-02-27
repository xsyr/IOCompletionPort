
#ifndef _Uncopyable_H_
#define _Uncopyable_H_

class Uncopyable
{
private:
	Uncopyable(const Uncopyable&);
	Uncopyable& operator=(const Uncopyable&);

protected:
	Uncopyable(){}
	~Uncopyable(){}
};

#endif