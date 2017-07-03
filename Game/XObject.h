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

	class CxObject
	{
	public:
		int64_t		m_tag; //�ڳ�����Ψһ�ı��
		string		m_name; //����
		int			m_icon;	//СФ����

		CxObject*  m_parent; //��

		set<CxObject*> m_childs;

	public:

		virtual ~CxObject();

		CxObject* ChildNew();
		void ChildDelete(CxObject* obj);
		void ChildRemoveAll();

	};


};




#endif // XObject_h__
