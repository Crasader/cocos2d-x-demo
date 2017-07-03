/*

 */

#ifndef _BYTEBUFFER_H
#define _BYTEBUFFER_H

//#include "Common.h"
//#include "Utilities/ByteConverter.h"
//#include "ByteConverter.h"

/** ByteConverter reverse your byte order.  This is use
for cross platform where they have different endians.
*/

#define MANGOS_LITTLEENDIAN 1
#define MANGOS_BIGENDIAN    0

//#if !defined(MANGOS_ENDIAN)
//#  if defined (ACE_BIG_ENDIAN)
//#    define MANGOS_ENDIAN MANGOS_BIGENDIAN
//#  else // ACE_BYTE_ORDER != ACE_BIG_ENDIAN
//#    define MANGOS_ENDIAN MANGOS_LITTLEENDIAN
//#  endif // ACE_BYTE_ORDER
//#endif // MANGOS_ENDIAN

//#include <Platform/Define.h>
#include <algorithm>
#include <set>
#include <list>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <stdint.h>
#include <string.h>

/*
typedef std::int64_t int64;
typedef std::int32_t int32;
typedef std::int16_t int16;
typedef std::int8_t int8;
typedef std::uint64_t uint64;
typedef std::uint32_t uint32;
typedef std::uint16_t uint16;
typedef std::uint8_t uint8;
*/

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;


namespace ByteConverter
{
	template<size_t T>
	inline void convert(char* val)
	{
		std::swap(*val, *(val + T - 1));
		convert < T - 2 >(val + 1);
	}

	template<> inline void convert<0>(char*) {}
	template<> inline void convert<1>(char*) {}             // ignore central byte

	template<typename T>
	inline void apply(T* val)
	{
		convert<sizeof(T)>((char*)(val));
	}
}

//#if MANGOS_ENDIAN == MANGOS_BIGENDIAN
#if (MANGOS_BIGENDIAN==1)
template<typename T> inline void EndianConvert(T& val) { ByteConverter::apply<T>(&val); }
template<typename T> inline void EndianConvertReverse(T&) { }
#else
template<typename T> inline void EndianConvert(T&) { }
template<typename T> inline void EndianConvertReverse(T& val) { ByteConverter::apply<T>(&val); }
#endif

template<typename T> void EndianConvert(T*);         // will generate link error
template<typename T> void EndianConvertReverse(T*);  // will generate link error

inline void EndianConvert(uint8&) { }
inline void EndianConvert(int8&) { }
inline void EndianConvertReverse(uint8&) { }
inline void EndianConvertReverse(int8&) { }


class ByteBufferException
{
	public:
		ByteBufferException(bool _add, size_t _pos, size_t _esize, size_t _size)
			: add(_add), pos(_pos), esize(_esize), size(_size)
		{
			PrintPosError();
		}

		void PrintPosError() const
		{

		}

	private:
		bool add;
		size_t pos;
		size_t esize;
		size_t size;
};


template<class T>
struct Unused
{
	Unused() {}
};

class ByteBuffer
{
	public:
		const static size_t DEFAULT_SIZE = 0x1000;

		// constructor
		ByteBuffer(): _rpos(0), _wpos(0)
		{
			_storage.reserve(DEFAULT_SIZE);
		}

		// constructor
		ByteBuffer(size_t res): _rpos(0), _wpos(0)
		{
			_storage.reserve(res);
		}

		// copy constructor
		ByteBuffer(const ByteBuffer& buf): _rpos(buf._rpos), _wpos(buf._wpos), _storage(buf._storage) { }

		void clear()
		{
			_storage.clear();
			_rpos = _wpos = 0;
		}

		template <typename T> void put(size_t pos, T value)
		{
			EndianConvert(value);
			put(pos, (uint8*)&value, sizeof(value));
		}

		ByteBuffer& operator<<(uint8 value)
		{
			append<uint8>(value);
			return *this;
		}

		ByteBuffer& operator<<(uint16 value)
		{
			append<uint16>(value);
			return *this;
		}

		ByteBuffer& operator<<(uint32 value)
		{
			append<uint32>(value);
			return *this;
		}

		ByteBuffer& operator<<(uint64 value)
		{
			append<uint64>(value);
			return *this;
		}

		// signed as in 2e complement
		ByteBuffer& operator<<(int8 value)
		{
			append<int8>(value);
			return *this;
		}

		ByteBuffer& operator<<(int16 value)
		{
			append<int16>(value);
			return *this;
		}

		ByteBuffer& operator<<(int32 value)
		{
			append<int32>(value);
			return *this;
		}

		ByteBuffer& operator<<(int64 value)
		{
			append<int64>(value);
			return *this;
		}

		// floating points
		ByteBuffer& operator<<(float value)
		{
			append<float>(value);
			return *this;
		}

		ByteBuffer& operator<<(double value)
		{
			append<double>(value);
			return *this;
		}

		ByteBuffer& operator<<(const std::string& value)
		{
			append((uint8 const*)value.c_str(), value.length());
			append((uint8)0);
			return *this;
		}


		ByteBuffer& operator<<(const char* str)
		{
			append((uint8 const*)str, str ? strlen(str) : 0);
			append((uint8)0);
			return *this;
		}


		//////////////////////////////////////////////////////////////////////////

		ByteBuffer& operator>>(bool& value)
		{
			value = read<char>() > 0 ? true : false;
			return *this;
		}

		ByteBuffer& operator>>(uint8& value)
		{
			value = read<uint8>();
			return *this;
		}

		ByteBuffer& operator>>(uint16& value)
		{
			value = read<uint16>();
			return *this;
		}

		ByteBuffer& operator>>(uint32& value)
		{
			value = read<uint32>();
			return *this;
		}

		ByteBuffer& operator>>(uint64& value)
		{
			value = read<uint64>();
			return *this;
		}

		// signed as in 2e complement
		ByteBuffer& operator>>(int8& value)
		{
			value = read<int8>();
			return *this;
		}

		ByteBuffer& operator>>(int16& value)
		{
			value = read<int16>();
			return *this;
		}

		ByteBuffer& operator>>(int32& value)
		{
			value = read<int32>();
			return *this;
		}

		ByteBuffer& operator>>(int64& value)
		{
			value = read<int64>();
			return *this;
		}

		ByteBuffer& operator>>(float& value)
		{
			value = read<float>();
			return *this;
		}

		ByteBuffer& operator>>(double& value)
		{
			value = read<double>();
			return *this;
		}

		ByteBuffer& operator>>(std::string& value)
		{
			value.clear();
			while (rpos() < size())                         // prevent crash at wrong string format in packet
			{
				char c = read<char>();
				if (c == 0)
					break;
				value += c;
			}
			return *this;
		}

		template<class T>
		ByteBuffer& operator>>(Unused<T> const&)
		{
			read_skip<T>();
			return *this;
		}


		uint8 operator[](size_t pos) const
		{
			return read<uint8>(pos);
		}

		size_t rpos() const { return _rpos; }

		size_t rpos(size_t rpos_)
		{
			_rpos = rpos_;
			return _rpos;
		}

		size_t wpos() const { return _wpos; }

		size_t wpos(size_t wpos_)
		{
			_wpos = wpos_;
			return _wpos;
		}

		template<typename T>
		void read_skip() { read_skip(sizeof(T)); }

		void read_skip(size_t skip)
		{
			if (_rpos + skip > size())
				throw ByteBufferException(false, _rpos, skip, size());
			_rpos += skip;
		}

		template <typename T> T read()
		{
			T r = read<T>(_rpos);
			_rpos += sizeof(T);
			return r;
		}

		template <typename T> T read(size_t pos) const
		{
			if (pos + sizeof(T) > size())
				throw ByteBufferException(false, pos, sizeof(T), size());
			T val = *((T const*)&_storage[pos]);
			EndianConvert(val);
			return val;
		}

		void read(uint8* dest, size_t len)
		{
			if (_rpos  + len > size())
				throw ByteBufferException(false, _rpos, len, size());
			memcpy(dest, &_storage[_rpos], len);
			_rpos += len;
		}

		uint64 readPackGUID()
		{
			uint64 guid = 0;
			uint8 guidmark = 0;
			(*this) >> guidmark;

			for (int i = 0; i < 8; ++i)
			{
				if (guidmark & (uint8(1) << i))
				{
					uint8 bit;
					(*this) >> bit;
					guid |= (uint64(bit) << (i * 8));
				}
			}

			return guid;
		}

		const uint8* contents() const { return &_storage[0]; }
		const char* c_str() { return (char*)&_storage[0]; }

		size_t size() const { return _storage.size(); }
		bool empty() const { return _storage.empty(); }

		void resize(size_t newsize)
		{
			_storage.resize(newsize);
			_rpos = 0;
			_wpos = size();
		}

		void reserve(size_t ressize)
		{
			if (ressize > size())
				_storage.reserve(ressize);
		}

		void append(const std::string& str)
		{
			append((uint8 const*)str.c_str(), str.size() + 1);
		}

		void append(const char* src, size_t cnt)
		{
			return append((const uint8*)src, cnt);
		}

		template<class T> void append(const T* src, size_t cnt)
		{
			return append((const uint8*)src, cnt * sizeof(T));
		}

		void append(const uint8* src, size_t cnt)
		{
			if (!cnt)
				return;

//            MANGOS_ASSERT(size() < 10000000);

			if (_storage.size() < _wpos + cnt)
				_storage.resize(_wpos + cnt);
			memcpy(&_storage[_wpos], src, cnt);
			_wpos += cnt;
		}

		void append(const ByteBuffer& buffer)
		{
			if (buffer.wpos())
				append(buffer.contents(), buffer.wpos());
		}

		// can be used in SMSG_MONSTER_MOVE opcode
		void appendPackXYZ(float x, float y, float z)
		{
			uint32 packed = 0;
			packed |= ((int)(x / 0.25f) & 0x7FF);
			packed |= ((int)(y / 0.25f) & 0x7FF) << 11;
			packed |= ((int)(z / 0.25f) & 0x3FF) << 22;
			*this << packed;
		}

		void appendPackGUID(uint64 guid)
		{
			uint8 packGUID[8 + 1];
			packGUID[0] = 0;
			size_t size = 1;
			for (uint8 i = 0; guid != 0; ++i)
			{
				if (guid & 0xFF)
				{
					packGUID[0] |= uint8(1 << i);
					packGUID[size] =  uint8(guid & 0xFF);
					++size;
				}

				guid >>= 8;
			}

			append(packGUID, size);
		}

		void put(size_t pos, const uint8* src, size_t cnt)
		{
			if (pos + cnt > size())
				throw ByteBufferException(true, pos, cnt, size());
			memcpy(&_storage[pos], src, cnt);
		}

//        void print_storage() const;
//        void textlike() const;
//        std::string hexlike() const;

		void fix_npk_size() {
			put<uint32_t>(0, (uint32_t)(size() - 4));
		}

		unsigned int length()
		{
				return (unsigned int) this->size();
		}
//		void seekp(unsigned long pos);

	private:
		// limited for internal use because can "append" any unexpected type (like pointer and etc) with hard detection problem
		template <typename T> void append(T value)
		{
			EndianConvert(value);
			append((uint8*)&value, sizeof(value));
		}

	protected:
		size_t _rpos, _wpos;
		std::vector<uint8> _storage;
};

//template <typename T>
//inline ByteBuffer& operator<<(ByteBuffer& b, std::vector<T> const& v)
//{
//    b << (uint32)v.size();
//    for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); ++i)
//    {
//        b << *i;
//    }
//    return b;
//}
//
//template <typename T>
//inline ByteBuffer& operator>>(ByteBuffer& b, std::vector<T>& v)
//{
//    uint32 vsize;
//    b >> vsize;
//    v.clear();
//    while (vsize--)
//    {
//        T t;
//        b >> t;
//        v.push_back(t);
//    }
//    return b;
//}
//
//template <typename T>
//inline ByteBuffer& operator<<(ByteBuffer& b, std::list<T> const& v)
//{
//    b << (uint32)v.size();
//    for (typename std::list<T>::iterator i = v.begin(); i != v.end(); ++i)
//    {
//        b << *i;
//    }
//    return b;
//}
//
//template <typename T>
//inline ByteBuffer& operator>>(ByteBuffer& b, std::list<T>& v)
//{
//    uint32 vsize;
//    b >> vsize;
//    v.clear();
//    while (vsize--)
//    {
//        T t;
//        b >> t;
//        v.push_back(t);
//    }
//    return b;
//}
//
//template <typename K, typename V>
//inline ByteBuffer& operator<<(ByteBuffer& b, std::map<K, V>& m)
//{
//    b << (uint32)m.size();
//    for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); ++i)
//    {
//        b << i->first << i->second;
//    }
//    return b;
//}
//
//template <typename K, typename V>
//inline ByteBuffer& operator>>(ByteBuffer& b, std::map<K, V>& m)
//{
//    uint32 msize;
//    b >> msize;
//    m.clear();
//    while (msize--)
//    {
//        K k;
//        V v;
//        b >> k >> v;
//        m.insert(make_pair(k, v));
//    }
//    return b;
//}

template<>
inline void ByteBuffer::read_skip<char*>()
{
	std::string temp;
	*this >> temp;
}

template<>
inline void ByteBuffer::read_skip<char const*>()
{
	read_skip<char*>();
}

template<>
inline void ByteBuffer::read_skip<std::string>()
{
	read_skip<char*>();
}





//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class ByteReader
{
protected:
	const char* dat;
	size_t _rpos;
	size_t _total_size;

public:
	ByteReader(const void* _src, size_t _Size)
	{
		_rpos = 0;
		dat = (const char*)_src;
		_total_size = _Size;
	};

	size_t rpos() const { return _rpos; }
	size_t size() const { return _total_size; }
	size_t remain() const { return _total_size - _rpos; };

	const char* Current() { return dat + _rpos; };

	template <typename T> T read(size_t pos) const
	{
		if (pos + sizeof(T) > size()) {
			//	throw ByteReaderException(false, pos, sizeof(T), size());

		}
		T val = *((T const*)&dat[pos]);
		EndianConvert(val);
		return val;
	}

	template <typename T> T read()
	{
		T r = read<T>(_rpos);
		_rpos += sizeof(T);
		return r;
	}


	const char* c_str() { return dat; }

	size_t readInc(int _offset)
	{
		_rpos += _offset;
#ifndef _MSC_VER		
		if (_rpos < 0) _rpos = 0;
#endif		
		return _rpos;
	}

	//////////////////////////////////////////////////////////////////////////

	ByteReader& operator >> (bool& value)
	{
		value = read<char>() > 0 ? true : false;
		return *this;
	}

	ByteReader& operator >> (uint8& value)
	{
		value = read<uint8>();
		return *this;
	}

	ByteReader& operator >> (uint16& value)
	{
		value = read<uint16>();
		return *this;
	}

	ByteReader& operator >> (uint32& value)
	{
		value = read<uint32>();
		return *this;
	}

	ByteReader& operator >> (uint64& value)
	{
		value = read<uint64>();
		return *this;
	}

	// signed as in 2e complement
	ByteReader& operator >> (int8& value)
	{
		value = read<int8>();
		return *this;
	}

	ByteReader& operator >> (int16& value)
	{
		value = read<int16>();
		return *this;
	}

	ByteReader& operator >> (int32& value)
	{
		value = read<int32>();
		return *this;
	}

	ByteReader& operator >> (int64& value)
	{
		value = read<int64>();
		return *this;
	}

	ByteReader& operator >> (float& value)
	{
		value = read<float>();
		return *this;
	}

	ByteReader& operator >> (double& value)
	{
		value = read<double>();
		return *this;
	}

	ByteReader& operator >> (std::string& value)
	{
		value.clear();
		while (rpos() < size())                         // prevent crash at wrong string format in packet
		{
			char c = read<char>();
			if (c == 0)
				break;
			value += c;
		}
		return *this;
	}


	uint64 readPackGUID()
	{
		uint64 guid = 0;
		uint8 guidmark = 0;
		(*this) >> guidmark;

		for (int i = 0; i < 8; ++i)
		{
			if (guidmark & (uint8(1) << i))
			{
				uint8 bit;
				(*this) >> bit;
				guid |= (uint64(bit) << (i * 8));
			}
		}

		return guid;
	}

	void read(uint8* dest, size_t len)
	{
		if (_rpos + len > size()) return;
		//			throw ByteBufferException(false, _rpos, len, size());
		memcpy(dest, &dat[_rpos], len);
		_rpos += len;
	}

	//////////////////////////////////////////////////////////////////////////


	template<typename T>
	void read_skip() { read_skip(sizeof(T)); }

	void read_skip(size_t skip)
	{
		if (_rpos + skip > size()) {
			//	throw ByteReaderException(false, _rpos, skip, size());
		}
		_rpos += skip;
	}

};

#define ByteReaderClass(a,b) a.read((uint8*)&b,sizeof(b))









#endif
