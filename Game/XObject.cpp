
#include "XObject.h"

namespace xg
{

	CxObject::~CxObject()
	{
		//清除所有的孩子
		ChildRemoveAll();
	}

	CxObject* CxObject::ChildNew()
	{
		CxObject* obj = new CxObject();
		obj->m_parent = this;
		m_childs.insert(obj);
		return obj;
	}


	void CxObject::ChildDelete(CxObject* obj)
	{
		if (obj) {
			m_childs.erase(obj);
			delete obj;
		}
	}

	void CxObject::ChildRemoveAll()
	{
		while (!m_childs.empty())
		{
			auto it = m_childs.begin();
			CxObject* obj = (*it);
			delete obj;
			m_childs.erase(it);
		}
	}

};

