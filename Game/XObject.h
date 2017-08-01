#ifndef XObject_h__
#define XObject_h__

#include <string>
#include <stdint.h>
#include <vector>
#include <set>
#include <map>
#include <time.h>


#include "Singleton.h"
#include "CxVar.h"

using namespace std;

enum XOBJ_ATTR_TYPE
{
	OAT_I8,
	OAT_I16,
	OAT_I32,  //	OAT_INT,  //��BOOL
	OAT_I64,

	OAT_FLOAT,
	OAT_DOUBLE,

	OAT_U8,
	OAT_U16,
	OAT_U32,
	OAT_U64,

	//������
	OAT_STRING,
	OAT_BLOB,

	//����
	OAT_VECTOR,
	OAT_MAP,

};

namespace xg
{

	class GxObject
	{
	public:
		int64_t		m_tag; //�ڳ�����Ψһ�ı��
		string		m_name; //����
		int			m_icon;	//СФ����

		uint16_t m_level;						// ��ɫ�ȼ�


		GxObject*  m_parent; //��

		set<GxObject*> m_childs;
		CxVarMap  m_attrib;

	public:

		virtual ~GxObject();

		GxObject* ChildNew();
		void ChildDelete(GxObject* obj);
		virtual void ChildRemoveAll();

	public:


		/**
		���ö��������е�Ԫ��ֵ
		@param kname ���� �������Vector������������
		@param _type Ԫ������
		@param v ��Ҫ����ֵ��ָ��
		@param vsz ��Ҫ����ֵ�����ݳ���
		@param _parent �������ĸ����ڵ��� ������δ�վ�Ϊ���ڵ�
		@param _force ����������Ϊ true  ��������Բ����ھʹ���
		*/
		virtual void* AttribSet(const char* kname, XOBJ_ATTR_TYPE _type, const char* v, size_t vsz, void* _parent = NULL, bool _force = true);

		virtual void* AttribGet(const char* kname, int& _type, void* _parent = NULL);

		/**
		��ʾ���е�ֵ
		*/
		virtual void* AttribGetValue(const char* kname, void* _parent = NULL);

		//ɾ������������
		virtual bool  AttribRemove(const char* kname, void* _parent = NULL);
		virtual void  AttribRemoveAll();
		virtual bool  AttribIsExist(const char* kname, void* _parent = NULL);
		/**
		���������� ֧�� :: �ָ�
		*/
		virtual void* AttribMap(const char* kname, void* _parent = NULL, bool _force = true);
	public:
		virtual void clear();

		template<class T>
		T AttribGetValueReal(const char* kname, void* _parent = NULL)
		{
			T r = { 0 };
			void* v = AttribGetValue(kname, _parent);
			if (v) memcpy(&r, v, sizeof(T));
			return r;
		}

	};


};




#endif // XObject_h__
