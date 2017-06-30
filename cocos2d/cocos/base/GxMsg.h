

#ifndef _GXMSG_H_
#define _GXMSG_H_


#include <deque>
#include <mutex>
#include <memory.h>

//#include "autorelease.h"

typedef unsigned int uint;

class GxMsg /*: public CxRef*/
{
	char* dat;
	uint   len;
	void* owner;

public:

	GxMsg(const void* buf,uint size,void* _who)
	{
//		autorelease();
		dat = NULL;
		len = size;
		owner = _who;
		if (len) {
			dat = new char[len + 1];
			dat[len] = '\0';
			memcpy(dat, buf, len);
		}
	}

	virtual ~GxMsg()
	{
		if (dat) delete[] dat;
//		fprintf(stderr, "--------释放内存=========++++++");
	}

	char* Data() { return dat; }
	uint  Length() { return len; }
	void* Who() { return owner; }

	GxMsg* Clone()
	{
		return new GxMsg(dat, len, owner);
	}

};


class GxMsgQueue
{
private:
	static GxMsgQueue* _share;

protected:
	std::deque<GxMsg*> workQue;
	std::deque<GxMsg*> waitQue;
	std::mutex* mtx_wq;

public:
	GxMsgQueue();

	static GxMsgQueue* shared();

	virtual ~GxMsgQueue();

	void PushBack(const void* buf, size_t size, void* who);


	void LoopWork(void(cbWork)(char *, size_t, void*, void *), void * arg);
	void clear();

	void ReleaseMsg(GxMsg* m);

};



#endif // !_GXMSG_H_





