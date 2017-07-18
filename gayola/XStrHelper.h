
#ifndef XStrHelper_h__
#define XStrHelper_h__

#include <string.h>
#include <string>
#include <sstream>
#include <vector>

namespace XStrHelper
{

	typedef unsigned char uint8;
	typedef unsigned int uint32;
	typedef unsigned char uchar;

	/**
	从文本串中获取其中的值 以第一个 [kname:]或者[kname=] 为首 以 [\n]或[//空格]或结束为 结束
	@return 返回其后面的字符串
	*/
	std::string GetUrlValueString(const char* _buf, size_t _size, std::string kname);
	bool GetValueStringFromURL(std::string& result, std::string url, std::string kname);

	/**
	从HTTP协议头部分获取键值串
	@param src 数据
	*/
	bool GetValueStringFromHttpHead(std::string& result, std::string src, std::string kname);

	template<class T>
	T ConvertTo(std::string& src) {
		T v;
		std::stringstream sstr;
		sstr << src;
		sstr.seekg(0);
		sstr >> v;
		return v;
	};

	template<class T>
	std::vector<uchar>& Append(std::vector<uchar>& src, const T& v)
	{
		size_t o_pos = src.size();
		src.resize(src.size() + sizeof(T));
		memcpy(&src[o_pos], &v, sizeof(T));
		return src;
	}

	std::vector<uchar>& AppendString(std::vector<uchar>& src, const std::string& v);


	/**
	把字符串中含有的 类 &#13; 转成 \r 
	*/
	std::string Decode255(std::string& str);


	/**
	URL编码
	把 像 %0d 转成 \r
	*/
	std::string DecodeURL(std::string& str);


	inline bool IsDigit(char c) {
		if (c >= '0' && c <= '9') return true;
		return false;
	};


	/**
	0..9 A..F a..f
	*/
	inline bool IsHexDigit(char c) {
		if (c >= '0' && c <= '9') return true;
		if (c >= 'a' && c <= 'f') return true;
		if (c >= 'A' && c <= 'F') return true;
		return false;
	}

	/**
	@return 正常 0..16 不在区间内返回 -1
	*/
	inline int HexValue(char c) {

		if (c >= '0' && c <= '9') return c-'0';
		if (c >= 'a' && c <= 'f') return c-'a'+10;
		if (c >= 'A' && c <= 'F') return c-'A'+10;

		return -1;
	}

	inline char HexToChar(char src[2]) {
		//
		int v= HexValue(src[0]) * 16 + HexValue(src[1]);
		return (char)v;
	}

	std::string hexEncodeByteArray(const uint8* bytes, size_t arrayLen, std::string& result);
	std::string hexDecodeByteArray(const uint8* bytes, size_t arrayLen, std::string& result);

	/**
	将字符串中符合的子串替换成新串
	*/
	std::string& ReplaceAll(std::string& str, std::string o_str, std::string n_str);


	std::string ws2s(const std::wstring& ws, char const* _Locale);

	#define	X_UTF8_BOM  { 0xefU,0xbbU,0xbfU,0 }

	

};


#endif // XStrHelper_h__