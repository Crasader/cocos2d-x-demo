
#include "XStrHelper.h"
#include <assert.h>

#ifdef _MSC_VER
#include <windows.h>
#endif

namespace XStrHelper
{

	/**
	从像URL的字符串中获取其中一个值
	*/
	std::string GetUrlValueString(const char* src, size_t size, std::string name)
	{
		assert(src);
		assert(!name.empty());

		std::string str;
		std::string strSrc;
		strSrc.append(src, size);


		//看name最后一个是否是　: or = 
		if (name.back() != ':' || name.back() != '=') name.push_back(':');

		std::string::size_type pos = strSrc.find(name);
		if (pos == std::string::npos) return str;

		//找从此后的第一个不是分隔符的内容为开始 而后找到是分割符或者结束为结束
		bool _start = false;
		const char* _endstr = src + size;
		for (const char* s = &strSrc[pos+name.length()]; s != _endstr; s++)
		{
			if ((s[0] == ':' || s[0] == '=') && _start == false) continue;
			if ((s[0] == ' '|| s[0] == '&' || s[0] == '\n') && _start) break;
			_start = true;
			str.push_back(s[0]);
		}

		return str;
	}

	std::string Decode255(std::string& str)
	{
		std::string::size_type k = 0;
		std::string::size_type pos = 0;
		char _value[8];
		char c=0;
		do 
		{
			pos = str.find("&#",pos);
			if (pos != std::string::npos) {
				//判断这个后面的第一个字符开始到第一个;之间是否为数字 并且不超过3个
				k = pos + 2;
				memcpy(_value, &str[k], 4);
				
				if (!IsDigit(_value[0])) { pos++; continue; }
				if (!IsDigit(_value[1])) { pos++; continue; }
				//第三个如果是 ; 就开始
				if (_value[2] == ';')
				{
					_value[2] = '\0';
					int _v = atoi(_value);
					c = (char)_v;

					//原来的字符串删除标记后插入新字符串
					//str.erase(str.begin()+ pos, str.begin() +pos+ 5);
					str.erase(pos, 5);
					str.insert(pos, &c, 1);
					pos++;
					continue;
				}
				
				//如果第三个还是数字 第四个就必须是 ;
				if (IsDigit(_value[2]) && _value[3] == ';')
				{
					_value[3] = '\0';
					int _v = atoi(_value);
					c = (char)_v;

					//原来的字符串删除标记后插入新字符串
					str.erase(pos, 6);
					str.insert(pos, &c, 1);
					pos++;
					continue;
				}

			}

		} while (pos != std::string::npos);
		
		return str;
	}

	/**
	URL编码
	把 像 %0d 转成 \r
	*/
	std::string DecodeURL(std::string& str)
	{

		std::string::size_type k = 0;
		std::string::size_type pos = 0;
		char _value[2];
		char c = 0;
		do
		{
			pos = str.find("%", pos);
			if (pos != std::string::npos) {
				//判断这个后面的第一个字符开始到第一个;之间是否为数字 并且不超过3个
				k = pos + 1;
				memcpy(_value, &str[k], 2);

				if (!IsHexDigit(_value[0])) { pos++; continue; }
				if (!IsHexDigit(_value[1])) { pos++; continue; }
				//是就开始
				//if (_value[2] == ';')
				{
					
					c = HexToChar(_value);

					//原来的字符串删除标记后插入新字符串
					str.erase(pos, 3);
					str.insert(pos, &c, 1);
					pos++;
					continue;
				}

			}

		} while (pos != std::string::npos);

		return str;
	}

	std::string hexEncodeByteArray(const uint8* bytes, size_t arrayLen, std::string& result)
	{
		if (&result == NULL) return "";

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
		return result;
	}

	std::string hexDecodeByteArray(const uint8* bytes, size_t arrayLen, std::string& result)
	{
		if (&result == NULL) return "";

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
		return result;
	}

	std::string& ReplaceAll(std::string& str, std::string o_str, std::string n_str)
	{
		size_t olen = o_str.length();
		size_t nlen = n_str.length();
		std::string::size_type pos = 0;
		do {
			pos = str.find(o_str,pos);
			if (pos != std::string::npos) {
				str.erase(pos, olen);
				if(nlen>0) str.insert(pos, n_str);
				pos++;
			}
		} while (pos != std::string::npos);
		return str;
	}


	// https://msdn.microsoft.com/en-us/library/k1f9b8cy.aspx

	std::string ws2s(const std::wstring& ws, char const* _Locale)
	{
		size_t   i;
		size_t requiredSize = 4096;
		std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
														 //	setlocale(LC_ALL, "chs"); //设置成中文
		if (_Locale == NULL) _Locale = "zh-CN";
		setlocale(LC_ALL, _Locale); //设置成中文
		const wchar_t* _Source = ws.c_str();

		/*
		#if defined(_WINDOWS)
		int rc = wcstombs_s(&requiredSize,NULL,ws.length(), _Source, 0) + 1; // C4996
		#else
		requiredSize = wcstombs(NULL, _Source, 0) + 1; // C4996
		#endif
		*/
		//requiredSize = wcstombs(NULL, _Source, 0) + 1; // C4996
		// Note: wcstombs is deprecated; consider using wcstombs_s

		//wcstombs_s(&requiredSize,0, NULL, _Source, INT32_MAX);
#if defined(_MSC_VER)
		wcstombs_s(&requiredSize, NULL, 0, _Source, 0);
		requiredSize += 1;
#else
		requiredSize = wcstombs(NULL, ws.c_str(), 0) + 1;
#endif

		requiredSize = 4096;

		//	size_t _Dsize = 2 * ws.size() + 1;
		char *_Dest = new char[requiredSize];
		//	memset(_Dest, 0, _Dsize);

#if defined(_MSC_VER)
		wcstombs_s(&i, _Dest, requiredSize - 1, _Source, requiredSize - 1);
#else
		wcstombs(_Dest, _Source, requiredSize);
#endif


		_Dest[requiredSize - 1] = '\0';

		std::string result = _Dest;
		delete[]_Dest;
		setlocale(LC_ALL, curLocale.c_str());

		return result;
	}

	bool GetValueStringFromURL(std::string& result, std::string url,std::string kname)
	{
		using namespace std;
		string::size_type pos = 0,posR=-1;

		if (kname.empty() || url.empty() ) return false;
		std::string _url=url;// = DecodeURL(url);
		
		if (kname.back() != '=') kname.push_back('=');

		pos = _url.find(kname);
		if (std::string::npos == pos) return false;

		size_t l = kname.length();
		//从这里找到 & 如果没找到就直接到尾部
		posR = _url.find('&', pos + l);
		if (posR > pos) {
			result = _url.substr(pos + l, posR - pos - l);
		}
		else result = _url.substr(pos + l);

		return true;
	}


	bool GetValueStringFromHttpHead(std::string& result, std::string src, std::string kname)
	{
		using namespace std;
		string::size_type pos = 0, posR = -1;

		if (kname.empty() || src.empty()) return false;
		std::string _url = src;// = DecodeURL(url);

		//if (kname.back() != '=') kname.push_back('=');

		pos = _url.find(kname);
		if (std::string::npos == pos) return false;

		size_t l = kname.length();
		//从这里找到 & 如果没找到就直接到尾部
		posR = _url.find('\n', pos + l);
		if (posR > pos) {
			result = _url.substr(pos + l, posR - pos - l);
		}
		else result = _url.substr(pos + l);

		//把前面 空格和:去掉
		// (std::string::size_type i=0;i<result.length();i++)
		while(!result.empty())
		{
			char c = result[0];
			if (c == ' ') {
				result.erase(result.begin());
				continue;
			}
			if (c == ':') {
				result.erase(result.begin());
			}
			break;
		}

		return true;
	}

	//MBs2WCs 函数的实现 GB2312 向 Unicode 的转换
	wchar_t * MBs2WCs(const char* pszSrc) {
		wchar_t* pwcs = NULL;
		int size = 0;
		#if defined(_linux_)
			setlocale(LC_ALL, "zh_CN.GB2312");
		size = mbstowcs(NULL, pszSrc, 0);
		pwcs = new wchar_t[size + 1];
		size = mbstowcs(pwcs, pszSrc, size + 1);
		pwcs[size] = 0;
#else
size = MultiByteToWideChar(20936, 0, pszSrc, -1, 0, 0);
if (size <= 0)
return NULL;
pwcs = new wchar_t[size];
MultiByteToWideChar(20936, 0, pszSrc, -1, pwcs, size);
#endif 
return pwcs;
	}

	char* WCs2MBs(const wchar_t * wcharStr) {
		char* str = NULL;
		int size = 0;
		#if defined(_linux_)
			setlocale(LC_ALL, "zh_CN.UTF8");
		size = wcstombs(NULL, wcharStr, 0);
		str = new char[size + 1];
		wcstombs(str, wcharStr, size);
		str[size] = '\0';
#else
size = WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, NULL, NULL, NULL, NULL);
str = new char[size];
WideCharToMultiByte(CP_UTF8, 0, wcharStr, -1, str, size, NULL, NULL);
#endif 
return str;
	}


	std::vector<uchar>& AppendString(std::vector<uchar>& src, const std::string& v)
	{
		if (v.empty()) return src;
		size_t o_pos = src.size();
		src.resize(src.size() + v.length());
		memcpy(&src[o_pos], v.c_str(), v.length());
		src.push_back('\0');
		return src;
	}




};
