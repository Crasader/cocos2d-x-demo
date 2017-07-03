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
		int64_t		m_tag; //�ڳ�����Ψһ�ı��
		string		m_name; //����
		int			m_icon;	//СФ����

		GxObject*  m_parent; //��

		set<GxObject*> m_childs;

	public:

		virtual ~GxObject();

		GxObject* ChildNew();
		void ChildDelete(GxObject* obj);
		void ChildRemoveAll();

	};


};




#endif // XObject_h__
