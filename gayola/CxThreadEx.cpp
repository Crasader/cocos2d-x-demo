
#include "CxThreadEx.h"

CxThreadPool* CxThreadPool::_instance = NULL;

CxThreadPool::~CxThreadPool()
{
	while (!works.empty())
	{
		auto it = works.begin();
		delete it->second;
		works.erase(it);
	}
}

CxThreadPool* CxThreadPool::Instance()
{
	if (_instance == NULL) _instance = new CxThreadPool();
	return _instance;
}

void CxThreadPool::Step()
{
	std::lock_guard<std::mutex> lck(mtx_class);
	while (!m_del_que.empty())
	{
		auto it = m_del_que.begin();
		std::thread* th = (*it);
		th->join();
		delete th;
		m_del_que.erase(it);
	}
}


void DIRECTOR_TICK_THREADPOOL(float,void*)
{
	CxThreadPool::Instance()->Step();
}
