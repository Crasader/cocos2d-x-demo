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
	OAT_I32,  //	OAT_INT,  //含BOOL
	OAT_I64,

	OAT_FLOAT,
	OAT_DOUBLE,

	OAT_U8,
	OAT_U16,
	OAT_U32,
	OAT_U64,

	//块数据
	OAT_STRING,
	OAT_BLOB,

	//容器
	OAT_VECTOR,
	OAT_MAP,

};

namespace xg
{

	class GxObject
	{
	public:
		int64_t		m_tag; //在场景中唯一的编号
		string		m_name; //名字
		int			m_icon;	//小肖像编号

		uint16_t m_level;						// 角色等级


		GxObject*  m_parent; //父

		set<GxObject*> m_childs;
		CxVarMap  m_attrib;

	public:

		virtual ~GxObject();

		GxObject* ChildNew();
		void ChildDelete(GxObject* obj);
		virtual void ChildRemoveAll();

	public:


		/**
		设置对象属性中的元素值
		@param kname 名称 如果父是Vector容器将被忽略
		@param _type 元素类型
		@param v 将要设置值的指针
		@param vsz 将要设置值的数据长度
		@param _parent 挂载在哪个父节点上 如果这个未空就为根节点
		@param _force 如果这个参数为 true  如果此属性不存在就创建
		*/
		virtual void* AttribSet(const char* kname, XOBJ_ATTR_TYPE _type, const char* v, size_t vsz, void* _parent = NULL, bool _force = true);

		virtual void* AttribGet(const char* kname, int& _type, void* _parent = NULL);
		virtual void* AttribGetValue(const char* kname, void* _parent = NULL);

		//删除对象属性项
		virtual bool  AttribRemove(const char* kname, void* _parent = NULL);
		virtual void  AttribRemoveAll();
		virtual bool  AttribIsExist(const char* kname, void* _parent = NULL);
		/**
		创建属性树 支持 :: 分割
		*/
		virtual void* AttribMap(const char* kname, void* _parent = NULL, bool _force = true);
	public:
		virtual void clear();

	};


};




#endif // XObject_h__
