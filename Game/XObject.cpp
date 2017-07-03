
#include "XObject.h"

namespace xg
{

	GxObject::~GxObject()
	{
		//������еĺ���
		ChildRemoveAll();
	}

	GxObject* GxObject::ChildNew()
	{
		GxObject* obj = new GxObject();
		obj->m_parent = this;
		m_childs.insert(obj);
		return obj;
	}


	void GxObject::ChildDelete(GxObject* obj)
	{
		if (obj) {
			m_childs.erase(obj);
			delete obj;
		}
	}

	void GxObject::ChildRemoveAll()
	{
		while (!m_childs.empty())
		{
			auto it = m_childs.begin();
			GxObject* obj = (*it);
			delete obj;
			m_childs.erase(it);
		}
	}

};

