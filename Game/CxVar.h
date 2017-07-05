
#ifndef CxVar_h__
#define CxVar_h__

/*
可进行存储优化压缩空间

2017-06-06 增加名称中支持  :: 分隔

*/

#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <map>
#include <list>
#include <unordered_map>
#include <set>
#include <string>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <locale.h>
#include <vector>

class CxVar;

enum VAR_TYPE
{

	V_TYPE_I8,
	V_TYPE_I16,
	V_TYPE_I32,  //	V_TYPE_INT,  //含BOOL
	V_TYPE_I64,

	V_TYPE_FLOAT,
	V_TYPE_DOUBLE,

	V_TYPE_U8,
	V_TYPE_U16,
	V_TYPE_U32,
	V_TYPE_U64,

	V_TYPE_STRING,
	V_TYPE_BLOB, //块数据

	V_TYPE_VECTOR,
	V_TYPE_MAP, //容器V_TYPE_FIELD,

};



typedef unsigned char uchar;
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef uint64_t uint64;
typedef int64_t int64;
typedef unsigned int uint;

#define XMEM_NEW(a,b,c) if (a == NULL) while (a == NULL) a = (char*)malloc(b + 1); else if(b>c) while (a == NULL) a = (char*)realloc(a, b + 1); a[b]=0

typedef std::vector<CxVar*> CxVarVector;
typedef std::map<std::string, CxVar*> CxVarMap;
typedef std::vector<char> CxArrayByte;

#define XAPPEND_VSTR(des,src,sz) for (uint32 i = 0; i < sz; i++) des.push_back(src[i])

class CxVar
{
protected:

	union
	{
		std::vector<CxVar*>* varVector;
		std::map<std::string, CxVar*>* varMap;
	};

	std::vector<char> m_dat;
	char m_type;	//数据类型
	uint32   m_size;	//内存长度 如果是容器这个就是容器的元素个数

public:

	CxVar()
	{
		varVector = NULL;
		varMap = NULL;
		m_size = 0;
		m_type = 0;
	}

	virtual ~CxVar()
	{
		Clear();
	}


	inline VAR_TYPE  GetType() { return (VAR_TYPE)m_type; }

	void SetType(VAR_TYPE t)
	{
		if (m_type == t) return;
		Clear();
		m_type = t;
	}


	/*
		CxVar& operator=(const CxVar& _Right)
		{
			Clear();

			m_type = _Right.m_type;


			if (m_type < V_TYPE_VECTOR)
			{
				XMEM_NEW(var, _Right.m_size, m_size);
				memcpy(var, _Right.var, _Right.m_size);
			}

			if (m_type == V_TYPE_VECTOR)
			{
				varVector = new CxVarVector();
				CloneVector(*varVector,*_Right.varVector);
			}

			if (m_type == V_TYPE_MAP)
			{
				varMap = new CxVarMap();
				CloneMap(*varMap, *_Right.varMap);
			}


			m_size = _Right.m_size;
			return *this;
		}
	*/

	template<class T, VAR_TYPE ty>
	int SetValue(const T& v)
	{
		//		assert(m_type > V_TYPE_STRING);
				//if (m_type >= V_TYPE_STRING) return -1;
		if (m_type > V_TYPE_BLOB) Clear();

		m_type = ty;
		m_size = sizeof(v);
		//XMEM_NEW(var, 8,m_size);
		//memcpy(var, &v, sizeof(v));
		m_dat.resize(m_size);
		memcpy(&m_dat[0], &v, m_size);
		return 0;
	}

	template<class T>
	int GetValue(T& v)
	{
		assert(m_type < V_TYPE_STRING);

		//memcpy(&v, var, sizeof(v));
		memcpy(&v, &m_dat[0], sizeof(T));
		return 0;
	}

	/*
		int SetString(const char* src, uint32 size)
		{
			if (src == NULL || size < 1) return -1;

			//assert(m_type == V_TYPE_STRING);
			if (m_type > V_TYPE_BLOB) Clear();
			m_type = V_TYPE_STRING;

			//if (var == NULL)
			//	while (var == NULL) var = (char*)malloc(size + 1);
			//else
			//	while (var == NULL) var = (char*)realloc(var, size + 1);
			XMEM_NEW(var, size,m_size);

			m_size = (uint32)size;
			//var[size] = 0;
			memcpy(var, src, size);

			return 0;
		}

		int GetString(std::string& s)
		{
			assert(m_type == V_TYPE_STRING);
			s.clear();
			s.append(var, m_size);
			return 0;
		}
	*/


	int SetString(const char* src, uint32 size)
	{
		if (src == NULL || size < 1) return -1;

		if (m_type > V_TYPE_STRING) Clear();
		m_type = V_TYPE_STRING;

		//XMEM_NEW(var, size,m_size);
		m_size = (uint32)size;
		//memcpy(var, src, size);
		m_dat.clear();
		//for (uint32 i = 0; i < size; i++) m_dat.push_back(src[i]);
		XAPPEND_VSTR(m_dat, src, size);

		return 0;
	}

	int GetString(std::string& s)
	{
		assert(m_type == V_TYPE_STRING);
		s.clear();
		s.append((char*)&m_dat[0], m_dat.size());
		return 0;
	}



	int SetBlob(const void* src, uint32 size)
	{
		if (src == NULL || size < 1) return -1;
		//assert(m_type==V_TYPE_BLOB);
		if (m_type > V_TYPE_BLOB) Clear();

		//		XMEM_NEW(var, size,m_size);
		m_size = size;
		//		memcpy(var, src, size);

		const char* s = (const char*)src;
		m_dat.resize(size);
		memcpy(&m_dat[0], src, size);

		return 0;
	}

	char* GetData() {
		assert(m_type < V_TYPE_VECTOR);
		return &m_dat[0];
	}

	uint32 Length() {
		return m_size;
	}

	//void SetLength(uint32 _sz)
	//{
	//	m_size = _sz;
	//}

	void Clear()
	{
		m_size = 0;

		//if (var && m_type<V_TYPE_VECTOR)
		//{
		//	free(var);
		//	var = NULL;
		//	return;
		//}

		if (m_type == V_TYPE_VECTOR) {
			ClearVector();
			return;
		}

		if (m_type == V_TYPE_MAP)
		{
			ClearMap();
			return;
		}

	}


	//std::string ToString()
	//{
	//}

	/**
	返回增加的长度
	*/
	size_t WriteToArrayByte(CxArrayByte& ar)
	{
		std::string slen;
		const size_t op = ar.size();
		//类型
		char c = m_type;
		ar.push_back(c);
		//		ar.push_back('\0');

				//内容
		if (m_type <= V_TYPE_U64)
		{
			//int j = 0;
			m_size = m_dat.size();
			int k = m_size - 1;
			while (k >= 0)
			{
				if (0 == m_dat[k]) {
					k--;
					continue;
				}
				break;
			}

			c = (char)(k + 1);
			ar.push_back(c);
			ArrayByteAppend(ar, &m_dat[0], c);
			//ar.push_back('\0');
			return ar.size() - op;
		}

		if (m_type == V_TYPE_STRING || m_type == V_TYPE_BLOB)
		{
			//长度
			slen = std::to_string(m_size);
			ArrayByteAppend(ar, slen.c_str(), slen.length());
			ar.push_back('\0');

			//内容
			ArrayByteAppend(ar, &m_dat[0], (size_t)m_size);

			return ar.size() - op;
		}


		//如果是Vector
		if (m_type == V_TYPE_VECTOR)
		{
			CxVarVectorWriteToArrayByte(ar, *varVector);
		}

		if (m_type == V_TYPE_MAP)
		{
			CxVarMapWriteToArrayByte(ar, *varMap);
		}

		return ar.size() - op;
	}


	uint32 LoadFromByte(const char* src)
	{
		uint32 op = 0;// +sizeof(m_size);
		std::string slen;

		char c = src[0];
		m_type = (VAR_TYPE)c;
		op = 1;

		//数字
		if (m_type <= V_TYPE_U64)
		{
			//XMEM_NEW(var, 8, 0);
			//memset(var, 0, 8);
			m_dat.resize(8);
			memset(&m_dat[0], 0, 8);
			c = src[1];
			for (char i = 0; i < c; i++)
			{
				m_dat[i] = src[2 + i];
			}
			op = (size_t)c + 2;
			return op;
		}


		//字串数据
		if (m_type == V_TYPE_STRING || m_type == V_TYPE_BLOB)
		{
			slen.clear();
			uint32 k = GetStringFromByte(&src[1], NULL);
			uint32 osz = m_size;
			m_size = (uint32)std::strtoull(&src[1], 0, 10);
			//XMEM_NEW(var, m_size, osz);
			//memcpy(var, &src[1+k], m_size);
			m_dat.resize(m_size);
			memcpy(&m_dat[0], &src[1 + k], m_size);
			return (k + 1 + m_size);
		}



		//memcpy(&m_size, &src[1], sizeof(m_size));
		//uint64 osz = m_size;

		//是Vector
		if (m_type == V_TYPE_VECTOR)
		{
			SetVector();
			op += CxVarVectorLoadFromByte(*varVector, &src[op]);
			m_size = (uint32)varVector->size();
			return op;
		}

		if (m_type == V_TYPE_MAP)
		{
			op = 1;
			SetMap();
			op += CxVarMapLoadFromByte(*varMap, &src[op]);
			return op;
		}

		return op;
	}

	/**
	op 读了多少字节
	*/
	static CxVar* createFromByte(const char* src, uint32& op)
	{
		CxVar* v = new CxVar();
		op = v->LoadFromByte(src);
		return v;
	}


	void DataFromByte(const void* buf, uint32 vsz)
	{
		assert(m_type < V_TYPE_VECTOR);
		//XMEM_NEW(var, vsz, m_size);
		m_size = vsz;
		//memcpy(var, buf, (size_t)m_size);
		m_dat.resize(m_size);
		memcpy(&m_dat[0], buf, m_size);
	}

public:

	bool ClearVector()
	{
		if (m_type != V_TYPE_VECTOR) return false;
		if (varVector) {
			for (auto it : (*varVector))
			{
				delete it;
			}
			varVector->clear();
			delete varVector;
			varVector = NULL;
			return true;
		}
		return false;
	}

	CxVarVector* GetVector()
	{
		if (m_type != V_TYPE_VECTOR || varVector == NULL) return NULL;
		return varVector;
	}


	CxVarVector* SetVector()
	{
		if (m_type == V_TYPE_VECTOR && varVector) return varVector;
		Clear();
		m_type = V_TYPE_VECTOR;
		varVector = new CxVarVector();
		return varVector;
	}

	//template<class T>
	//int VectorPushBack(const T& v)
	//{
	//	if (v == NULL) return -2;
	//	CxVarVector* vv=GetVector();
	//	if (vv == NULL) return -1;

	//	CxVar* vi = new CxVar();
	//	vi->SetValue<T>(v);
	//	vv->push_back(vi);
	//	return 0;
	//}

	template<class T, VAR_TYPE ty>
	static CxVar* VectorPushBack(CxVarVector& vv, const T& v)
	{
		CxVar* vi = new CxVar();
		vi->SetValue<T, ty>(v);
		vv.push_back(vi);
		return vi;
	}

	//int VectorRemove(CxVar* v)
	//{
	//}

	static void CxVarVectorWriteToArrayByte(CxArrayByte& ar, CxVarVector& vv)
	{
		//元素个数
		//uint64 as =(uint64) vv.size();
		//ArrayByteAppend(ar, &as, sizeof(as));

		std::string slen = std::to_string(vv.size());
		ArrayByteAppend(ar, slen.c_str(), slen.length());
		ar.push_back('\0');

		for (auto it : vv)
		{
			it->WriteToArrayByte(ar);
		}
	}

	static uint32 CxVarVectorLoadFromByte(CxVarVector& vv, const char* src)
	{
		uint32 op = 0;
		//先读出多少个元素
		uint32 k = GetStringFromByte(src, NULL);
		op = k;
		uint32 sz = (uint32)std::strtoull(src, NULL, 10);
		for (uint32 i = 0; i < sz; i++)
		{
			uint32 opp = 0;
			vv.push_back(CxVar::createFromByte(src + op, opp));
			op += opp;
		}

		return op;
	}



public:
	bool ClearMap()
	{
		if (m_type != V_TYPE_MAP) return false;
		if (varMap) {
			for (auto it : (*varMap))
			{
				delete it.second;
			}
			varMap->clear();
			delete varMap;
			varMap = NULL;
			return true;
		}
		return false;
	}

	CxVarMap* GetMap()
	{
		if ((m_type != V_TYPE_MAP) || varMap == NULL) return NULL;
		return varMap;
	}

	CxVarMap* SetMap()
	{
		if (m_type == V_TYPE_MAP && varMap) return varMap;

		Clear();
		m_type = V_TYPE_MAP;
		varMap = new CxVarMap();
		return varMap;
	}

	void BindMap(CxVarMap* _vm)
	{
		Clear();
		m_type = V_TYPE_MAP;
		varMap = _vm;
	}

	//	void CopyFrom(const CxVar* _var);


	static void CxVarMapWriteToArrayByte(CxArrayByte& ar, CxVarMap& vm)
	{
		std::string slen = std::to_string(vm.size());
		ArrayByteAppend(ar, slen.c_str(), slen.length());
		ar.push_back('\0');

		for (auto it : vm) {
			//if (it.first.empty()) continue;
			ArrayByteAppend(ar, it.first.c_str(), it.first.length());
			ar.push_back(0);
			it.second->WriteToArrayByte(ar);
		}
	}

	static uint32 CxVarMapLoadFromByte(CxVarMap& vm, const char* src)
	{
		std::string slen;
		uint32 op = 0;
		//先读出多少个元素
		uint32 k = GetStringFromByte(src, &slen);
		op = k;
		uint32 sz = (uint32)std::strtoull(src, NULL, 10);
		for (uint32 i = 0; i < sz; i++)
		{
			std::string kname;
			uint32 opp = 0;
			//先读出字符串 然后读出内容
			opp = GetStringFromByte(src + op, &kname);
			op += opp;

			CxVar* v1 = new CxVar();
			opp = v1->LoadFromByte(src + op);
			op += opp;
			//vv.push_back(CxVar::createFromByte(src + op, opp));

			vm[kname] = v1;
		}

		return op;
	}

public:
	//////////////////////////////////////////////////////////////////////////

	bool MapAddItem(std::string kname, CxVar* v)
	{
		CxVarMap* vm = GetMap();
		if (vm == NULL) return false;

		int64 pos = kname.rfind("::");
		if (pos != std::string::npos)
		{
			std::string strFirstName = kname.substr(0, (uint)pos);
			std::string strRName = kname.substr((uint)pos + 2);

			CxVar* vv = MapAddTree(*vm, strFirstName);
			(*vv->varMap)[strRName] = vv;
			return true;
		}
		else
		{
			if (vm) {
				(*vm)[kname] = v;
				return true;
			}
		}
		return false;
	}


	bool MapRemove(std::string kname) {
		CxVarMap* vm = GetMap();
		if (vm == NULL) return false;

		//进行名称 :: 检查
		std::string strFName = kname;
		std::string strSName = kname;

		int64 pos = kname.rfind("::");
		if (pos != std::string::npos)
		{
			strFName = kname.substr(0, (uint)pos);
			strSName = kname.substr((uint)pos + 2);

			CxVar* vv = MapAddTree(*vm, strFName, false);
			if (vv)
			{
				vm = vv->varMap;
				goto cc2;
			}
			return false;
		}

	cc2:

		auto it = vm->find(strSName);
		if (it != vm->end())
		{
			CxVar* _vd = it->second;
			vm->erase(it);
			delete _vd;
			return true;
		}
		return false;
	}


	static CxVar* MapGetItem(CxVarMap& vm, std::string kname, bool _force = true)
	{
		CxVar* v = NULL;
		CxVarMap* pvm = &vm;

		//进行名称 :: 检查
		std::string strFName = kname;
		std::string strSName = kname;
		int64 pos = kname.rfind("::");
		if (pos != std::string::npos)
		{
			strFName = kname.substr(0, (uint)pos);
			strSName = kname.substr((uint)pos + 2);

			CxVar* vv = MapAddTree(vm, strFName, _force);
			if (vv)
			{
				pvm = vv->varMap;
				goto cc2;
			}
			return NULL;
		}

	cc2:

		auto it = pvm->find(strSName);
		if (it != pvm->end()) return it->second;

		if (_force)
		{
			v = new CxVar();
			(*pvm)[strSName] = v;
		}
		return v;
	}


	template<class T, VAR_TYPE ty>
	static CxVar* MapAdd(CxVarMap& vm, std::string kname, const T& val, bool _force = true)
	{
		CxVarMap* pvm = &vm;
		//进行名称 :: 检查
		std::string strFName = kname;
		std::string strSName = kname;
		int64 pos = kname.rfind("::");
		if (pos != std::string::npos)
		{
			strFName = kname.substr(0, (uint)pos);
			strSName = kname.substr((uint)pos + 2);

			CxVar* vv = MapAddTree(vm, strFName, _force);
			if (vv)
			{
				pvm = vv->varMap;
				goto cc2;
			}
			return NULL;
		}

	cc2:
		CxVar* v = new CxVar();
		v->SetValue<T, ty>(val);
		(*pvm)[strSName] = v;
		return v;
	}

	/**
	用 :: 分割开名称
	*/
	static CxVar* MapAddTree(CxVarMap& vm, std::string kname, bool _force = true)
	{
		CxVar* result = NULL;
		std::string strFirstName = kname;
		std::string strRightName = kname;
		bool bt = false;
		//这里针对名字中的 :: 进行分割 然后再进行处理
		int64 pos = kname.find("::");
		if (pos != std::string::npos)
		{
			bt = true;
			strFirstName = kname.substr(0, (uint)pos);
			strRightName = kname.substr((uint)pos + 2);
		}

		auto it = vm.find(strFirstName);
		if (it == vm.end()) {

			if (!_force) return NULL;

			result = CxVar::createMap();
			vm[strFirstName] = result;
		}
		else {
			result = it->second;
		}

		if (!bt)	goto cc2;
		return MapAddTree(*result->varMap, strRightName, _force);

	cc2:
		return result;
	}

	template<class T, VAR_TYPE ty>
	static CxVar* MapSetItem(CxVarMap& vm, const std::string kname, const T& v, bool _force)
	{
		CxVar* v1 = MapAdd<T, ty>(vm, kname, v, _force);
		if (v1)
		{
			v1->SetValue<T, ty>(v);
			return v1;
		}

		return NULL;
	}


	static CxVar* MapSetItemString(CxVarMap& vm, std::string kname, std::string v, bool _force)
	{
		CxVar* v1 = MapGetItem(vm, kname, _force);
		if (v1)
		{
			v1->SetString(v.c_str(), v.length());
			return v1;
		}
		return NULL;
	}

	static void MapItemClear(CxVarMap& vm)
	{
		for (auto it : vm) {
			delete it.second;
		}
		vm.clear();
	}

	static CxVarMap* MapGetSubByName(CxVarMap& vm, std::string kname)
	{
		CxVar* result = MapAddTree(vm, kname, false);
		if (result)
		{
			return result->varMap;
		}
		return NULL;
	}

	static bool MapItemRemove(CxVarMap& vm, std::string kname)
	{
		CxVarMap* pvm = NULL;
		CxVar* v1 = MapGetItem(vm, kname, false);
		if (v1)
		{
			std::string strMname = kname;
			std::string strKname = kname;
			int64 pos = kname.rfind("::");
			if (pos != std::string::npos)
			{
				strMname = kname.substr(0, (uint)pos);
				strKname = kname.substr((uint)pos + 2);
				pvm = MapGetSubByName(vm, strMname);//删除其中的项目
			}
			else {
				pvm = &vm;
			}

			pvm->erase(strKname);
			delete v1;
			return true;
		}
		return false;
	}

	static int MapWriteToArrayByteEx(CxArrayByte& ar, CxVarMap& vm)
	{
		char c = V_TYPE_MAP;
		ar.push_back(c);
		CxVarMapWriteToArrayByte(ar, vm);
		return 0;
	}



public:

	//	int LoadFromFile(std::string fname);
	//	int SaveToFile(std::string fname);

public:



	//	virtual bool operator==(const CxVar& _Right) const;
	//	virtual bool operator!=(const CxVar& _Right) const;


	static int CloneVector(CxVarVector& a, CxVarVector& b)
	{

		return 0;
	}


	static int CloneMap(CxVarMap& a, CxVarMap& b)
	{
		a.clear();

		return 0;
	}


public:

	//	friend std::ostream &operator<<(std::ostream&, CxVar&);
	//	friend std::istream &operator>>(std::istream&, CxVar&);


	static CxVar* createMap()
	{
		CxVar* v = new CxVar();
		v->m_type = V_TYPE_MAP;
		v->varMap = new CxVarMap();
		return v;
	}

	static CxVar* createVector()
	{
		CxVar* v = new CxVar();
		v->SetVector();
		return v;
	}

	static void hexEncodeByteArray(const uint8* bytes, size_t arrayLen, std::string& result)
	{
		if (&result == NULL) return;

		std::ostringstream ss;
		for (uint32 i = 0; i < arrayLen; ++i)
		{
			for (uint8 j = 0; j < 2; ++j)
			{
				unsigned char nibble = 0x0F & (bytes[i] >> ((1 - j) * 4));
				char encodedNibble;
				if (nibble < 0x0A)
					encodedNibble = '0' + nibble;
				else
					encodedNibble = 'A' + nibble - 0x0A;
				ss << encodedNibble;
			}
		}
		result = ss.str();
	}


	static void hexDecodeByteArray(const uint8* bytes, size_t arrayLen, std::string& result)
	{
		if (&result == NULL) return;

		result.clear();

		for (uint32 i = 0; i < arrayLen; ++i)
		{

			int value;
			if (bytes[i] >= '0' && bytes[i] <= '9')
				value = (bytes[i] - '0') << 4;
			else if (bytes[i] >= 'a' && bytes[i] <= 'f')
				value = (bytes[i] - 'a' + 10) << 4;
			else if (bytes[i] >= 'A' && bytes[i] <= 'F')
				value = (bytes[i] - 'A' + 10) << 4;
			else
				break;

			i++;
			if (bytes[i] >= '0' && bytes[i] <= '9')
				value |= bytes[i] - '0';
			else if (bytes[i] >= 'a' && bytes[i] <= 'f')
				value |= bytes[i] - 'a' + 10;
			else if (bytes[i] >= 'A' && bytes[i] <= 'F')
				value |= bytes[i] - 'A' + 10;
			else
				break;

			char ch = value;
			result.push_back(ch);
		}
	}


	static void ArrayByteAppend(CxArrayByte& des, const void* src, size_t size)
	{
		const char* s = (const char*)src;
		for (size_t i = 0; i < size; i++)
		{
			des.push_back(s[i]);
		}
	}

	static inline uint32 GetStringFromByte(const char* s, std::string* str)
	{
		uint32 k = 0;

		const char* s1 = s;
		do {
			char c = s1[0];
			if (c == 0) break;
			if (str) str->push_back(c);
			s1++;
			k++;
		} while (1);
		k++;
		return k;
	}



	static CxVar* createWithType(VAR_TYPE _type, uint32 vsz)
	{
		CxVar* v = new CxVar();
		v->m_type = _type;
		//XMEM_NEW(v->var, vsz, 0);
		v->m_size = vsz;
		v->m_dat.resize(vsz);
		return v;
	}



};




#endif // CxVar_h__
