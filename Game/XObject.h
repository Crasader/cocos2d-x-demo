#ifndef XObject_h__
#define XObject_h__

#include <string>
#include <stdint.h>
#include <vector>
#include <set>
#include <map>
#include <time.h>


#include "Singleton.h"

using namespace std;

namespace xg
{

	class GxObject
	{
	public:
		int64_t		m_tag; //在场景中唯一的编号
		string		m_name; //名字
		int			m_icon;	//小肖像编号

		GxObject*  m_parent; //父

		set<GxObject*> m_childs;

	public:

		virtual ~GxObject();

		GxObject* ChildNew();
		void ChildDelete(GxObject* obj);
		void ChildRemoveAll();

	};


};




#endif // XObject_h__
