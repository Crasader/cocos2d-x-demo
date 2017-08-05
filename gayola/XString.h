#ifndef XString_h__
#define XString_h__


#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

#include <list>
#include <map>
#include <unordered_map>
#include <stdint.h>

#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <assert.h>
#include <cstdarg>
#include <time.h>
#include <ctype.h>
#include <sstream>
#include <algorithm>

#include <functional> 
#include <cctype>
#include <locale>


#include "utf8util.h"

typedef unsigned char uchar;

class XStrUtil
{
public:

	/**
	获取命令行形式的字符串关键字后续的字符串
	xxx name yyyy
	返回 yyyy
	*/
	static std::string GetArgNameAfter(const char* src, const char* name);

	/**
	获取命令行形式的字符串关键字后续的字符串
	xxx name:yyyy
	返回 yyyy
	*/
	static std::string GetArgNameValue(const char* src, const char* name);

	static std::string GetTimeString(time_t _t);

	template<class T>
	static T Convert(std::string& str)
	{
		T v;
		std::stringstream sstr;
		sstr << str;
		sstr.seekg(0);
		sstr >> v;
		return v;
	}

	static size_t ReplaceAll(std::string& str, std::string old_str, std::string new_str);

	static size_t ReplaceForSQLComment(std::string& str);

	/**
	找到 %s 的位置 判断其长度 累加 其他的留 256
	效率比较低 注意谨慎使用
	*/
	static std::string createWithFormat(const char* fmt, ...);
	static std::string XStrUtil::createWithBool(bool v);

	static size_t TrimSpaceLeftAndRight(std::string& str);
	/**
	去除后面的空格
	*/
	static size_t TrimSpaceRight(std::string& str);

	/**
	去除前面的空格
	*/
	static size_t TrimSpaceLeft(std::string& str);

	//static std::string createWithTime(time_t t);

};


class XStringList
{
public:

	std::vector<size_t> lines;
	std::string m_dat; //内容存储

public:
	XStringList() {}
	virtual ~XStringList() {}

protected:

	const char* m_xendl;
	const char* m_xclose;

	void ParseString(const char* _spchar = "\r\n", const char* _close = "\"");

public:

	//    ssize_t Parse(const char* src);
	//    ssize_t Parse2(const char* src,size_t fl);

	// _spchar 分隔字符串
	//    size_t ParseEx(const char* lpBuf,size_t lpBufSize,const char* _spchar);

	/*
	_close 如果遇到这个符号 分割符号将被跳过，直到遇到第二个分割符号
	*/
	size_t ParseEx2(const char* lpBuf, size_t lpBufSize = 0, const char* _spchar = "\r\n", const char* _close = "\"");

public:
	void FixABSpace();//删除前后的空格
public:

	static void Test();

	int GetFeildLineVauleInt(std::string keyname);
	std::string GetFeildLineVauleString(std::string keyname);
	//    ssize_t GetVaulePos(const char* fieldn);

	void List();

	//    void Add(const char* text); //增加一行
	void push_back(std::string str);

public:
	/**
	行数
	*/
	size_t size();

	size_t LoadFromFile(const char* fname);
	size_t LoadFromString(std::string src, const char* lineMark = "\r\n", const char* _closeMark = "\"");

	virtual std::string operator[](size_t idx);
	std::string at(size_t idx);

	std::string GetLineString(size_t idx) {
		return at(idx);
	}


	//从头开始 返回完全符合这个词的后面一个词串
	//string NextLine(string str);

};



class XTokenizer : public XStringList
{
public:
	XTokenizer() {};

	//	XTokenizer(const std::string& _str, const std::string _delim=" ");
	//	XTokenizer(const char* _str,std::size_t sz=0, const std::string _delim=" ");

	virtual ~XTokenizer() {};

	//   int         countTokens();
	//   bool        hasMoreTokens();
	//   std::string nextToken();
	//   int         nextIntToken();
	//   double      nextFloatToken();
	//   std::string nextToken(const std::string& delim);
	//   std::string remainingString();
	//   std::string filterNextToken(const std::string& filterStr);

	/**
	*
	* */
	const char* GetTokenValue(std::string keyname);

	int GetTokenValueInt(const std::string& keyname);
	double  GetTokenValueFloat(const std::string& keyname);

	//第几个位置上
	int GetTokenValueInt(unsigned int _pos);

	std::string GetString(unsigned int _pos);
	std::string GetValueStringByIndex(unsigned int _idx);
	int GetValueIntByIndex(unsigned int _idx);
	/**
	* 返回关键字后面的内容
	*/
	std::string GetTokenRemain(const std::string& keyname);

	/**
	* 是否存在
	* */
	bool IsExistToken(const std::string& keyname);

	size_t GetTokenIndex(const std::string& keyname);


	//	size_t Parse(const char* lpBuf,ssize_t lpBufSize=-1,const char* _spchar=" ", char _close='"');

	void Print();

	bool FirstTokenIs(const std::string keyname);
	bool SecondTokenIs(const std::string keyname);

	//忽略大小写
	bool FirstTokenIsIcase(const std::string keyname);

	/**
	返回行数
	*/
	size_t NumLines() { return lines.size(); };

	size_t Parse(const char* buf, size_t sz, const char* sp = " \r\n", const char cc = '"');

	/**
	过滤掉都是换行字符的段
	*/
	//size_t Parse3(const char* buf, size_t sz, const char* sp = " \r\n", const char cc = '"');

	void KickNLine(const char* sp = " \r\n");

public:
	//		string m_dat;
	//   std::list<string>  token_str;
	//   std::string  delim;
	static std::string TokenClear(std::string str, std::string rstr = " \r\n", int _type = 1);
	std::string GetTokenValueString(const std::string& keyname);


	template<class T>
	T GetValue(int idx) {
		T v;
		std::string str = GetValueStringByIndex(idx);
		std::stringstream ss;
		ss << str;
		ss.seekg(0);
		ss >> v;
		return v;
	}

protected:
	//	std::list<string>::iterator cur_token;

};

//void test_XTokenizer();

/**
文本参数
#字母开头的本行注释掉
=分开前后 如果行中不存在 = 抛弃
*/
class XTextParamList
{

	std::map<std::string, std::string> valueMap;
protected:

	void ParseLine(std::string line);

public:
	static std::string GetValue(std::string filename, std::string keyname);

	void LoadFromAsciiFile(std::string filename);
	void Parse(const char* _lpBuf, size_t _size);

	std::string GetValue(std::string keyname);
	std::wstring GetValueWString(std::string keyname);

	bool IsExist(std::string keyname);

	void Print();

	void LoadFile(const char* filename);

};

class XLanguage : public XTextParamList
{
private:
	static XLanguage* _instance;

public:
	int CodePage;

public:
	static XLanguage* Instance();


};

class XTextFile
{
protected:
	uchar* dat;
	size_t		len;

public:
	static FILE* callfopen(const char* filepath, const char* mode)
	{
		assert(filepath);
		assert(mode);
#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (!defined WINCE)
		FILE* fp = 0;
		errno_t err = fopen_s(&fp, filepath, mode);
		if (err) {
			return 0;
		}
#else
		FILE* fp = fopen(filepath, mode);
#endif
		return fp;
	}

public:
	XTextFile();
	virtual ~XTextFile();

	bool LoadFile(const char* filename);
	int LoadFile(FILE*);

	//operator std::string();
	const char* c_str() { return (const char*)dat; };
	size_t size() { return len; };

};

namespace XStringUtil
{
	//bool utf8CharToUcs2Char(const char* utf8Tok, wchar_t* ucs2Char, unsigned int* utf8TokLen);
	//void ucs2CharToUtf8Char(const wchar_t ucs2Char, char* utf8Tok);
	//std::wstring utf8ToUcs2(const std::string& utf8Str);
	//std::string ucs2ToUtf8(const std::wstring& ucs2Str);

	typedef std::vector<std::string> Tokens;

	typedef int64_t int64;
	typedef int32_t int32;
	typedef int16_t int16;
	typedef int8_t int8;
	typedef uint64_t uint64;
	typedef uint32_t uint32;
	typedef uint16_t uint16;
	typedef uint8_t uint8;

	enum TimeConstants
	{
		MINUTE = 60,
		HOUR = MINUTE * 60,
		DAY = HOUR * 24,
		WEEK = DAY * 7,
		MONTH = DAY * 30,
		YEAR = MONTH * 12,
		IN_MILLISECONDS = 1000
	};

	Tokens StrSplit(const std::string& src, const std::string& sep);
	uint32 GetUInt32ValueFromArray(Tokens const& data, uint16 index);
	float GetFloatValueFromArray(Tokens const& data, uint16 index);

	void stripLineInvisibleChars(std::string& src);

	bool Utf8toWStr(const std::string& utf8str, std::wstring& wstr);
	// in wsize==max size of buffer, out wsize==real string size
	bool Utf8toWStr(char const* utf8str, size_t csize, wchar_t* wstr, size_t& wsize);
	inline bool Utf8toWStr(const std::string& utf8str, wchar_t* wstr, size_t& wsize)
	{
		return Utf8toWStr(utf8str.c_str(), utf8str.size(), wstr, wsize);
	}

	bool WStrToUtf8(std::wstring wstr, std::string& utf8str);
	// size==real string size
	bool WStrToUtf8(wchar_t* wstr, size_t size, std::string& utf8str);

	size_t utf8length(std::string& utf8str);                    // set string to "" if invalid utf8 sequence
	void utf8truncate(std::string& utf8str, size_t len);

	inline bool isBasicLatinCharacter(wchar_t wchar)
	{
		if (wchar >= L'a' && wchar <= L'z')                     // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
			return true;
		if (wchar >= L'A' && wchar <= L'Z')                     // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
			return true;
		return false;
	}

	inline bool isExtendedLatinCharacter(wchar_t wchar)
	{
		if (isBasicLatinCharacter(wchar))
			return true;
		if (wchar >= 0x00C0 && wchar <= 0x00D6)                 // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
			return true;
		if (wchar >= 0x00D8 && wchar <= 0x00DF)                 // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
			return true;
		if (wchar == 0x00DF)                                    // LATIN SMALL LETTER SHARP S
			return true;
		if (wchar >= 0x00E0 && wchar <= 0x00F6)                 // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
			return true;
		if (wchar >= 0x00F8 && wchar <= 0x00FE)                 // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
			return true;
		if (wchar >= 0x0100 && wchar <= 0x012F)                 // LATIN CAPITAL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK
			return true;
		if (wchar == 0x1E9E)                                    // LATIN CAPITAL LETTER SHARP S
			return true;
		return false;
	}

	inline bool isCyrillicCharacter(wchar_t wchar)
	{
		if (wchar >= 0x0410 && wchar <= 0x044F)                 // CYRILLIC CAPITAL LETTER A - CYRILLIC SMALL LETTER YA
			return true;
		if (wchar == 0x0401 || wchar == 0x0451)                 // CYRILLIC CAPITAL LETTER IO, CYRILLIC SMALL LETTER IO
			return true;
		return false;
	}

	inline bool isEastAsianCharacter(wchar_t wchar)
	{
		if (wchar >= 0x1100 && wchar <= 0x11F9)                 // Hangul Jamo
			return true;
		if (wchar >= 0x3041 && wchar <= 0x30FF)                 // Hiragana + Katakana
			return true;
		if (wchar >= 0x3131 && wchar <= 0x318E)                 // Hangul Compatibility Jamo
			return true;
		if (wchar >= 0x31F0 && wchar <= 0x31FF)                 // Katakana Phonetic Ext.
			return true;
		if (wchar >= 0x3400 && wchar <= 0x4DB5)                 // CJK Ideographs Ext. A
			return true;
		if (wchar >= 0x4E00 && wchar <= 0x9FC3)                 // Unified CJK Ideographs
			return true;
		if (wchar >= 0xAC00 && wchar <= 0xD7A3)                 // Hangul Syllables
			return true;
		if (wchar >= 0xFF01 && wchar <= 0xFFEE)                 // Halfwidth forms
			return true;
		return false;
	}

	inline bool isWhiteSpace(char c)
	{
		return ::isspace(int(c)) != 0;
	}

	inline bool isNumeric(wchar_t wchar)
	{
		return (wchar >= L'0' && wchar <= L'9');
	}

	inline bool isNumeric(char c)
	{
		return (c >= '0' && c <= '9');
	}

	inline bool isNumericOrSpace(wchar_t wchar)
	{
		return isNumeric(wchar) || wchar == L' ';
	}

	inline bool isNumeric(char const* str)
	{
		for (char const* c = str; *c; ++c)
			if (!isNumeric(*c))
				return false;

		return true;
	}

	inline bool isNumeric(std::string const& str)
	{
		for (std::string::const_iterator itr = str.begin(); itr != str.end(); ++itr)
			if (!isNumeric(*itr))
				return false;

		return true;
	}

	inline bool isNumeric(std::wstring const& str)
	{
		for (std::wstring::const_iterator itr = str.begin(); itr != str.end(); ++itr)
			if (!isNumeric(*itr))
				return false;

		return true;
	}

	inline bool isBasicLatinString(const std::wstring& wstr, bool numericOrSpace)
	{
		for (size_t i = 0; i < wstr.size(); ++i)
			if (!isBasicLatinCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
				return false;
		return true;
	}

	inline bool isExtendedLatinString(const std::wstring& wstr, bool numericOrSpace)
	{
		for (size_t i = 0; i < wstr.size(); ++i)
			if (!isExtendedLatinCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
				return false;
		return true;
	}

	inline bool isCyrillicString(const std::wstring& wstr, bool numericOrSpace)
	{
		for (size_t i = 0; i < wstr.size(); ++i)
			if (!isCyrillicCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
				return false;
		return true;
	}

	inline bool isEastAsianString(const std::wstring& wstr, bool numericOrSpace)
	{
		for (size_t i = 0; i < wstr.size(); ++i)
			if (!isEastAsianCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
				return false;
		return true;
	}

	inline void strToUpper(std::string& str)
	{
		//	std::transform(str.begin(), str.end(), str.begin(), toupper);

		char s = 'A' - 'a';
		for (size_t i = 0; i < str.length(); i++) {
			char c = str[i];
			//::uppercase(c);
			if (c >= 'a' && c <= 'z') str[i] = c + s;
		}

	}

	inline void strToLower(std::string& str)
	{
		//	std::transform(str.begin(), str.end(), str.begin(), tolower);
		char s = 'a' - 'A';
		for (size_t i = 0; i < str.length(); i++) {
			char c = str[i];
			//::uppercase(c);
			if (c >= 'A' && c <= 'Z') str[i] = c + s;
		}

	}

	inline wchar_t wcharToUpper(wchar_t wchar)
	{
		if (wchar >= L'a' && wchar <= L'z')                     // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
			return wchar_t(uint16(wchar) - 0x0020);
		if (wchar == 0x00DF)                                    // LATIN SMALL LETTER SHARP S
			return wchar_t(0x1E9E);
		if (wchar >= 0x00E0 && wchar <= 0x00F6)                 // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
			return wchar_t(uint16(wchar) - 0x0020);
		if (wchar >= 0x00F8 && wchar <= 0x00FE)                 // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
			return wchar_t(uint16(wchar) - 0x0020);
		if (wchar >= 0x0101 && wchar <= 0x012F)                 // LATIN SMALL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK (only %2=1)
		{
			if (wchar % 2 == 1)
				return wchar_t(uint16(wchar) - 0x0001);
		}
		if (wchar >= 0x0430 && wchar <= 0x044F)                 // CYRILLIC SMALL LETTER A - CYRILLIC SMALL LETTER YA
			return wchar_t(uint16(wchar) - 0x0020);
		if (wchar == 0x0451)                                    // CYRILLIC SMALL LETTER IO
			return wchar_t(0x0401);

		return wchar;
	}

	inline wchar_t wcharToUpperOnlyLatin(wchar_t wchar)
	{
		return isBasicLatinCharacter(wchar) ? wcharToUpper(wchar) : wchar;
	}

	inline wchar_t wcharToLower(wchar_t wchar)
	{
		if (wchar >= L'A' && wchar <= L'Z')                     // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
			return wchar_t(uint16(wchar) + 0x0020);
		if (wchar >= 0x00C0 && wchar <= 0x00D6)                 // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
			return wchar_t(uint16(wchar) + 0x0020);
		if (wchar >= 0x00D8 && wchar <= 0x00DE)                 // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
			return wchar_t(uint16(wchar) + 0x0020);
		if (wchar >= 0x0100 && wchar <= 0x012E)                 // LATIN CAPITAL LETTER A WITH MACRON - LATIN CAPITAL LETTER I WITH OGONEK (only %2=0)
		{
			if (wchar % 2 == 0)
				return wchar_t(uint16(wchar) + 0x0001);
		}
		if (wchar == 0x1E9E)                                    // LATIN CAPITAL LETTER SHARP S
			return wchar_t(0x00DF);
		if (wchar == 0x0401)                                    // CYRILLIC CAPITAL LETTER IO
			return wchar_t(0x0451);
		if (wchar >= 0x0410 && wchar <= 0x042F)                 // CYRILLIC CAPITAL LETTER A - CYRILLIC CAPITAL LETTER YA
			return wchar_t(uint16(wchar) + 0x0020);

		return wchar;
	}

	inline void wstrToUpper(std::wstring& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), wcharToUpper);
	}

	inline void wstrToLower(std::wstring& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), wcharToLower);
	}

	std::wstring GetMainPartOfName(std::wstring wname, uint32 declension);

	//bool utf8ToConsole(const std::string& utf8str, std::string& conStr);
	//bool consoleToUtf8(const std::string& conStr, std::string& utf8str);
	bool Utf8FitTo(const std::string& str, std::wstring search);


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

		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;

		return -1;
	}

	inline char HexToChar(char src[2]) {
		//
		int v = HexValue(src[0]) * 16 + HexValue(src[1]);
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


#endif // XString_h__
