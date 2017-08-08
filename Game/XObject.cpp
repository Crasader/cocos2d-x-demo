
#include "XObject.h"

namespace xg
{

	GxObject::~GxObject()
	{
		clear();

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
			m_childs.erase(it);
			delete obj;
		}
	}



	void GxObject::clear()
	{
		//清除所有的孩子
		ChildRemoveAll();

		AttribRemoveAll();


	}


	//////////////////////////////////////////////////////////////////////////

	void * GxObject::AttribSet(const char * kname, XOBJ_ATTR_TYPE _type, const char * v, size_t vsz,
		void * _parent, bool _force)
	{
		CxVar* var = NULL;
		CxVarMap* varMap = NULL;

		if (_parent == NULL) {
			varMap = &m_attrib;
		}
		else {
			CxVar* vv = (CxVar*)_parent;
			varMap = vv->GetMap();
		}

		var = CxVar::MapGetItem(*varMap, kname, _force);
		if (var) {
			var->SetType((VAR_TYPE)_type);
			var->DataFromByte(v, vsz);
			return var;
		}
		return NULL;
	}

	void * GxObject::AttribGet(const char * kname, int & _type, void * _parent)
	{
		CxVar* var = NULL;
		CxVarMap* varMap = NULL;

		if (_parent == NULL) {
			varMap = &m_attrib;
		}
		else {
			CxVar* vv = (CxVar*)_parent;
			varMap = vv->GetMap();
		}

		var = CxVar::MapGetItem(*varMap, kname, false);
		if (var) {
			_type = var->GetType();
			return var;
		}
		return NULL;

	}

	void * GxObject::AttribGetValue(const char * kname, void * _parent)
	{
		int t = 0;
		CxVar* v = (CxVar*)AttribGet(kname, t, _parent);
		if (v) return v->GetData();
		return nullptr;
	}

	void * GxObject::AttribGetValueEx(const char * kname,size_t& sz, void * _parent)
	{
		int t = 0;
		CxVar* v = (CxVar*)AttribGet(kname, t, _parent);
		if (v) {
			sz = v->Length();
			return v->GetData();
		}
		sz = 0;
		return nullptr;
	}

	std::string GxObject::AttribGetString(const char* kname, void* _parent /*= NULL*/)
	{
		int t = 0;
		CxVar* v = (CxVar*)AttribGet(kname, t, _parent);
		if (v) {
			size_t sz = v->Length();
			//return v->GetData();
			std::string s;
			s.append(v->GetData(), sz);
			return s;
		}
		return "";
	}

	bool GxObject::AttribRemove(const char * kname, void * _parent)
	{
		CxVar* var = NULL;
		CxVarMap* varMap = NULL;
		if (_parent == NULL) {
			varMap = &m_attrib;
		}
		else {
			CxVar* vv = (CxVar*)_parent;
			varMap = vv->GetMap();
		}

		CxVar::MapItemRemove(*varMap, kname);

		return true;
	}

	void GxObject::AttribRemoveAll()
	{
		while (!m_attrib.empty())
		{
			auto it = m_attrib.begin();
			CxVar* v = it->second;
			if (v) delete v;
			m_attrib.erase(it);
		}
	}

	bool GxObject::AttribIsExist(const char * kname, void * _parent)
	{
		CxVarMap* varMap = (CxVarMap*)_parent;
		if (varMap == NULL) varMap = &m_attrib;

		CxVar* var = CxVar::MapGetItem(*varMap, kname, false);
		return (var != NULL);
	}

	void* GxObject::AttribMap(const char* kname, void* _parent /*= NULL*/, bool _force /*= true*/)
	{
		CxVar* var = NULL;
		CxVarMap* varMap = NULL;

		if (_parent == NULL) {
			varMap = &m_attrib;
		}
		else {
			CxVar* vv = (CxVar*)_parent;
			varMap = vv->GetMap();
		}

		var = CxVar::MapAddTree(*varMap, kname, _force);
		if (var) {
			return var;
		}
		return NULL;
	}


	//////////////////////////////////////////////////////////////////////////

};

