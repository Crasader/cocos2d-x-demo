#ifndef XObject_h__
#define XObject_h__

#include <string>
#include <stdint.h>
#include <vector>
#include <set>
#include <map>

using namespace std;

namespace xg
{

	class CxObject
	{
	public:
		int64_t		m_tag; //在场景中唯一的编号
		string		m_name; //名字
		int			m_icon;	//小肖像编号

		CxObject*  m_parent; //父

		set<CxObject*> m_childs;

	public:

		virtual ~CxObject();

		CxObject* ChildNew();
		void ChildDelete(CxObject* obj);
		void ChildRemoveAll();

	};


};




#endif // XObject_h__
