#ifndef CxThreadEx_h__
#define CxThreadEx_h__


#include <mutex>
#include <thread>
#include <map>
#include <vector>


class CxThreadPool
{
protected:
	std::map<std::thread::id, std::thread*> works;
	std::mutex mtx_class;

	std::vector<std::thread*> m_del_que;

	CxThreadPool() {};

public:
	virtual ~CxThreadPool();

	void Add(std::thread* _work)
	{
		if (_work)
		{
			std::lock_guard<std::mutex> lck(mtx_class);
			works[_work->get_id()] = _work;
		}
	}

	void Release(std::thread::id _id)
	{
		auto it = works.find(_id);
		if (it != works.end())
		{
			std::lock_guard<std::mutex> lck(mtx_class);
			//delete it->second;
			m_del_que.push_back(it->second);
			works.erase(it);
		}
	}

	void Step();

	static CxThreadPool* Instance();

private:
	static CxThreadPool* _instance;

};

void DIRECTOR_TICK_THREADPOOL(float,void*);

#endif // CxThreadEx_h__
