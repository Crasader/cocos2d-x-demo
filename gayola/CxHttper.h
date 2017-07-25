#ifndef CxHttper_h__
#define CxHttper_h__

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unordered_map>

using namespace std;

#ifdef _MSC_VER
#define strcasecmp stricmp
#define strncasecmp  strnicmp
#endif


class CxHtpResponse
{
public:
	int m_nError;	//错误号

	//内容头上的字段
	std::unordered_map<std::string, std::string>	 m_feilds;

	std::string m_body;//内容主体


public:

	/**
	分析收到HTTP返回的内容
	*/
	bool Parse(const char* buf, size_t sz);

	/**
	变成一个文本
	强制加上内容长度标记 避免浏览器一直等待状态
	*/
	static std::string ToString(std::string _body)
	{
		std::string html;

		//temp
		html = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\r\n")
			+ "Content-Length: " + std::to_string(_body.length()) + "\r\n"
			+ "\r\n"
			+ _body;

		return html;
	}


};

class CxHttpHead
{
public:
	std::string Document;
	std::unordered_map<std::string, std::string> m_QueryParam;
	
	//协议头
	std::unordered_map<std::string, std::string> m_HeadFeilds;

	std::string Path;

public:
	CxHttpHead()
	{
	}

	virtual ~CxHttpHead()
	{
	}

public:

	static inline bool IsDigit(char c) {
		if (c >= '0' && c <= '9') return true;
		return false;
	};


	/**
	0..9 A..F a..f
	*/
	static inline bool IsHexDigit(char c) {
		if (c >= '0' && c <= '9') return true;
		if (c >= 'a' && c <= 'f') return true;
		if (c >= 'A' && c <= 'F') return true;
		return false;
	}

	/**
	@return 正常 0..16 不在区间内返回 -1
	*/
	static inline int HexValue(char c) {

		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;

		return -1;
	}

	static inline char HexToChar(char src[2]) {
		//
		int v = HexValue(src[0]) * 16 + HexValue(src[1]);
		return (char)v;
	}


	/**
	URL编码
	把 像 %0d 转成 \r
	*/
	static	std::string DecodeURL(std::string& str)
	{

		int k = 0;
		int pos = 0;
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

public:

	void ParseHeadFields(std::string url)
	{
		std::vector<std::string> vec;
		split(url, "\r\n", vec);

		for (auto e : vec)
		{
			std::vector<std::string> _vec;
			split(e, ":", _vec);
			if (_vec.size() < 2) continue; //避免崩溃
										 //处理URL编码
			trimString(_vec[0], "\r\n ");
			trimString(_vec[1], "\r\n ");
			//DecodeURL(_vec[0]);
			//DecodeURL(_vec[1]);
			m_HeadFeilds[_vec[0]] = _vec[1];
		}

	}

/*



*/

	int ParseGet(std::string& url)
	{
		//分析协议头上的字段
		ParseHeadFields(url);

		// GET /login.php?usr=%llu&pwd=%s&pcode=%d&app=%s&t=1&cip=%s&realm=%d& HTTP/1.1\r\n

		//找到第一个空格
		int path_start = url.find(" ")+1;
		int path_end = url.find(" ", path_start);

		Path = url.substr(path_start, path_end - path_start);

		int pos1 = Path.find('?');
		if (pos1 == -1) {
			Document = Path;
			return 0;
		}

		Document = Path.substr(0, pos1);

		std::string content = Path.substr(Document.length()+1);

		std::vector<std::string> vec;
		split(content, "&", vec);

		for (auto e : vec)
		{
			std::vector<std::string> _vec;
			split(e, "=", _vec);
			if(_vec.size()<2) continue; //避免崩溃
			//处理URL编码
			DecodeURL(_vec[0]);
			DecodeURL(_vec[1]);
			m_QueryParam[_vec[0]] = _vec[1];
		}



		return 0;
	}

	int GetKeyValueInt(const char * key)
	{
		/*
		auto it = m_map.find(std::string(key));
		if (it == m_map.end()) return -1;
		return atoi(it->second.c_str());
		*/
		std::string str = GetKeyValueString(key);
		if (str.empty()) return 0;
		return atoi(str.c_str());
	}

	std::string GetKeyValueString(const char * key)
	{
/*
		auto it = m_map.find(std::string(key));
		if (it == m_map.end()) return std::string();
		return it->second;
*/
		for (auto it : m_QueryParam)
		{
			if (strcasecmp(key, it.first.c_str()) == 0) return it.second;
		}
		return "";
	}

	std::string GetHeadValueString(const char * key)
	{
		for (auto it : m_HeadFeilds)
		{
			if (strcasecmp(key, it.first.c_str()) == 0) return it.second;
		}
		return "";
	}

private:
	void split(const string& src, const string& separator, vector<string>& dest)
	{
		string str = src;
		string substring;
		string::size_type start = 0, index;

		do
		{
			index = str.find_first_of(separator, start);
			if (index != string::npos)
			{
				substring = str.substr(start, index - start);
				dest.push_back(substring);
				start = str.find_first_not_of(separator, index);
				if (start == string::npos) return;
			}
		} while (index != string::npos);

		//the last token
		substring = str.substr(start);
		dest.push_back(substring);
	}

	/**
	修剪掉前后的内容
	*/
	static int trimString(std::string& src, std::string estr)
	{
		int r = 0;
		//前
		while (!src.empty())
		{
			char c = src.front();
			if (std::string::npos == estr.find(c)) break;
			src.erase(src.begin());
			r++;
		}

		//后
		while (!src.empty())
		{
			char c = src.back();
			if (std::string::npos == estr.find(c)) break;
			src.pop_back();
			r++;
		}

		return r;
	}


public:
	std::string GetDocumentType() {
		int pos = Document.rfind(".");
		if (pos > 0) {
			return Document.substr(pos);
		}
		return "";
	}

};


class CxHttper
{
public:

	static void EncodeURL(std::string& src)
	{
		//将保留字转换成%0d

	}

};


#endif // CxHttper_h__
