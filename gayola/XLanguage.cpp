

/*

IDS_ERROR_LOGIN_U_P= 用户名或密码错误

没有做特殊处理，没有做字符集编码转换

*/


#include "XLanguage.h"
#include <unordered_map>

namespace XLanguage
{
	static int app_encoding_code=86;
	using namespace  std;
	static unordered_map<string, string> CntStringNV;

	/**
	删除前后的 空格 \r \n
	*/
	string& trimLR(string& src)
	{
		while (!src.empty() &&( src.front() == ' ' || src.front() == '\r' || src.front() == '\n'))
		{
			src.erase(src.begin());
		}

		while (!src.empty() && (  src.back() == ' ' || src.back() == '\r' || src.back() == '\n'))
		{
			src.pop_back();
		}

		return src;
	}


	bool is_exclude(char c)
	{
		if (c == ' ') return true;
		if (c == '\r') return true;
		if (c == '\n') return true;
		return false;
	}

	const char* GetString(std::string idn)
	{
		auto it = CntStringNV.find(idn);
		if (it != CntStringNV.end()) return it->second.c_str();
		return "";
	}

	int Parse(const char* txt, size_t sz)
	{
		if (txt == NULL || sz==0) return -1;
		string strN, strV;
		bool _bLine = false;
		bool _bName = false;

		for (size_t i = 0; i < sz; i++)
		{
			char c = txt[i];
			if (c == '=') {
				if(strN.empty()) continue;
				_bName = true;
				continue;
			}

			if (c == '\r' || c == '\n' || i==sz-1)
			{
				if(strV.empty()) continue;
				if(strN.empty()) continue;
				CntStringNV[strN] = strV;
				strV.clear();
				strN.clear();
			}

			if (_bName) {
				strV.push_back(c);
			}
			else {
				strN.push_back(c);
			}
		}
		return 0;
	}

	int Clear()
	{
		CntStringNV.clear();
		return 0;
	}

	int GetCurrentCode()
	{
		return app_encoding_code;
	}

	int SetCurrentCode(int _code) {
		return app_encoding_code = _code;
	}

};

