#include "GxMsg.h"

GxMsgQueue* GxMsgQueue::_share = NULL;

GxMsgQueue::GxMsgQueue()
{
	mtx_wq = new std::mutex();
}

GxMsgQueue* GxMsgQueue::shared()
{
	if (_share==NULL) _share = new GxMsgQueue();
	return _share;
}

//#include "XLog.h"


GxMsgQueue::~GxMsgQueue()
{
	clear();
//	XLOG_DEBUG("释放消息队列");
	delete mtx_wq;
}

void GxMsgQueue::PushBack(const void * buf, size_t size, void * who)
{
	if (size < 1 || buf==NULL) return;

	std::lock_guard<std::mutex> lck(*mtx_wq);
	GxMsg* m = new GxMsg(buf, size, who);
	waitQue.push_back(m);
//	workQue.push_back(m);
}

void GxMsgQueue::LoopWork(void(cbWork)(char *,size_t,void*, void *), void * arg)
{
//	mtx_wq.lock();
	std::lock_guard<std::mutex> lck(*mtx_wq);
	while (!waitQue.empty())
	{
		GxMsg* m = waitQue.front();
	
#if(1)
		workQue.push_back(m);
#else
		workQue.push_back(m->Clone());
		m->release();
#endif
		waitQue.pop_front();
	}
	
	while (!workQue.empty())
	{
		GxMsg* m = workQue.front();
		if (cbWork) (cbWork)(m->Data(),m->Length(),m->Who(),arg);
		//m->release();
		ReleaseMsg(m);
		workQue.pop_front();
	}

//	mtx_wq.unlock();

}

void GxMsgQueue::clear()
{
	std::lock_guard<std::mutex> lck(*mtx_wq);
	while (!waitQue.empty()) {
		GxMsg* m = waitQue.front();
		//m->release();
		ReleaseMsg(m);
		waitQue.pop_front();
	}

	while (!workQue.empty())
	{
		GxMsg* m = workQue.front();
		//m->release();
		ReleaseMsg(m);
		workQue.pop_front();
	}
}

void GxMsgQueue::ReleaseMsg(GxMsg* m)
{
	delete m;
}
