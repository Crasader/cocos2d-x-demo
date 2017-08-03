
#include "XString.h"

#include "utf8util.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

using namespace std;

/**
获取命令行形式的字符串关键字后续的字符串
xxx name yyyy
返回 yyyy
*/
std::string XStrUtil::GetArgNameAfter(const char* src, const char* name)
{
	assert(src);
	assert(name);
	std::string str;
	const char* v = strstr(src, name);
	size_t l = strlen(src);

	if (v == NULL) return str;
	bool _start = false;
	for (const char* s = v + strlen(name); s != src + l; s++)
	{
		if (s[0] == ' ' && _start == false) continue;
		if (s[0] == ' ' && _start) break;
		_start = true;
		str.push_back(s[0]);
	}
	return str;
}

/**
获取命令行形式的字符串关键字后续的字符串
xxx name:yyyy
返回 yyyy
*/
std::string XStrUtil::GetArgNameValue(const char* src, const char* name)
{
	assert(src);
	assert(name);
	std::string str;
	const char* v = strstr(src, name);
	size_t l = strlen(src);

	if (v == NULL) return str;
	bool _start = false;
	for (const char* s = v + strlen(name); s != src + l; s++)
	{
		if ((s[0] == ':' || s[0] == '=') && _start == false) continue;
		if (s[0] == ' ' && _start) break;
		_start = true;
		str.push_back(s[0]);
	}
	return str;
}


std::string XStrUtil::GetTimeString(time_t _t)
{
	char buf[32];
	if ((int)_t == 0) _t = time(NULL);
	struct tm tt = { 0 };
#if defined(_MSC_VER)
	localtime_s(&tt, &_t);
#else //linux
	localtime_r(&_t, &tt);
#endif
	strftime(buf, 32, "%Y-%m-%d %H:%M%S", &tt);
	return buf;
}


size_t XStrUtil::ReplaceAll(string& str, string old_str, string new_str)
{
	size_t rc = 0;
	size_t ol = old_str.length();
	//		size_t nl = new_str.length();

	while (1)
	{
		std::string::size_type p = str.find(old_str);
		if (p == std::string::npos) break;
		str.replace(str.begin() + p, str.begin() + p + ol, new_str);
		rc++;
	}

	return rc;
}


size_t XStrUtil::ReplaceForSQLComment(string& str)
{
	size_t rc = ReplaceAll(str, "--", "&#45&#45");
	rc += ReplaceAll(str, "/*", "&#47&#42");
	rc += ReplaceAll(str, "*/", "&#42&#47");
	return rc;
}

/**
找到 %s 的位置 判断其长度 累加 其他的留 256
效率比较低 注意谨慎使用
*/
std::string XStrUtil::createWithFormat(const char* fmt, ...)
{
	int rs = 0;
	const char* _start = NULL;
	const char* _end = NULL;
	size_t total = 256;
	va_list ap;
	va_start(ap, fmt);
	_end = fmt;

	do
	{
		_start = strstr(_end, "%");
		if (_start == NULL) break;
		_end = _start + 1;
		if (_end[0] == '%') continue;

		if (_end[0] == 's') {
			total += strlen(va_arg(ap, const char*));
		}
		else {
			total += 256;
			va_arg(ap, int);
		}

	} while (1);
	va_end(ap);

	va_start(ap, fmt);
	char *buf = new char[total + 1];
	//memset(buf, 0, total + 1);
#ifdef _MSC_VER
	rs = ::_vsnprintf_s_l(buf, total - 1, total - 1, fmt, NULL, ap);
	//rs = vsprintf(buf, fmt, ap);
	//rs = vsnprintf(buf, total - 1, fmt, ap);
#else
	rs = vsnprintf(buf, total - 1, fmt, ap);
#endif
	buf[rs] = 0;
	std::string s;
	s.append(buf, rs);
	delete[] buf;

	va_end(ap);
	return s;
}



std::string XStrUtil::createWithBool(bool v)
{
	return v ? "true" : "false";
}

/**
去除前面的空格
*/
size_t XStrUtil::TrimSpaceLeft(std::string& str)
{
	size_t k = 0;
	//string::iterator i;
	//for (i = str.begin(); i != str.end(); i++) {
	//	if (!::isspace(*i)) {
	//		break;
	//	}
	//	k++;
	//}
	//if (i == str.end()) {
	//	str.clear();
	//}
	//else {
	//	if(i!=str.begin()) str.erase(str.begin(), i);
	//}
	string ch;
	for (size_t i = 0; i < str.length(); i++)
	{
		ch = str.substr(i, 1);
		if (ch.compare(" ") == 0) { k++; continue; }
		break;
	}
	if (k>0) str.erase(0, k);
	return k;
}

/**
去除后面的空格
*/
size_t XStrUtil::TrimSpaceRight(std::string& str)
{
	size_t k = 0;
	//string::iterator i;
	//for (i = str.end() - 1; ; i--) {
	//	if (!isspace(*i)) {
	//		if(i+1!=str.end()) str.erase(i + 1, str.end());
	//		break;
	//	}
	//	k++;
	//	if (i == str.begin()) {
	//		str.clear();
	//		break;
	//	}
	//}
	string ch;
	size_t i = str.length() - 1;
	for (; i >= 0; i--)
	{
		ch = str.substr(i, 1);
		if (ch.compare(" ") == 0) {
			k++; continue;
		}
		break;
	}
	if (k>0) str.erase(i + 1);
	return k;
}

size_t XStrUtil::TrimSpaceLeftAndRight(std::string& str)
{
	//return 0;
	return TrimSpaceLeft(str) + TrimSpaceRight(str);
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void XStringList::ParseString(const char* _spchar /*= "\r\n"*/, const char* _close /*= '"'*/)
{
	if (m_dat.empty()) return;

	if (_spchar == NULL) _spchar = "\r\n";
	if (_close == NULL) _close = "\"";

	m_xendl = _spchar;
	m_xclose = _close;


	//	ABM abm;
	lines.clear();
	//    aryString.clear();

	//	size_t lpBufSize = m_dat.length();
	const char* p = m_dat.c_str();
	//	const char* e = m_dat.c_str() + m_dat.length();

	//	const char* c = NULL;// strstr(p, _close);
	//	const char* l = NULL;// strstr(p, _spchar);

	//if (l < c) {
	//}

	//	std::string s1;
	size_t _el = strlen(_spchar);
	size_t _cl = strlen(_close);

	//	std::string::size_type pos = 0,pos1=0,pos_e=0;
	//	size_t lpos = 0;

	//	pos = m_dat.find("\r\n");

	bool c_mark = false;
	size_t i = 0;

	for (; i < m_dat.length(); i++)
	{
		const char* cc = strstr(p + i, _close);
		if (cc == p + i) c_mark = !c_mark;

#if(1)
		const char* ll = strstr(p + i, _spchar);
#else
		//TODO 取其中的一个分割符
		const char* ll = NULL;
		for (size_t j = 0; j < _el; j++)
		{
			char c[2];
			c[0] = _spchar[j];
			c[1] = 0;
			ll = strstr(p + i, c);
			if (ll) break;
		}
#endif		
		if (ll >= m_dat.c_str() + m_dat.length()) break;

		if (ll == p + i && !c_mark)
		{
			lines.push_back(i + _el);
			i += _el;
		}
	}

	if (lines.size() == 0) lines.push_back(m_dat.length());

	if (lines.size()>0 && lines.back() != m_dat.length())
	{
		lines.push_back(m_dat.length());
	}

	//	size_t ep = m_dat.length();




}




size_t XStringList::ParseEx2(const char* lpBuf, size_t lpBufSize, const char* _spchar, const char* _close)
{
	if (lpBufSize<1) lpBufSize = strlen(lpBuf);

	m_dat.clear();
	m_dat.append(lpBuf, lpBufSize);

	ParseString(_spchar, _close);

	return lines.size();

}

//删除前后的空格
void XStringList::FixABSpace()
{
	/*
	char c=0;
	ABM abm;
	for (size_t i=0; i<lines.size(); i++) {
	abm = lines[i];
	//去掉前面的
	for (size_t k=abm.a; k<abm.b; k++) {
	c=m_dat[k];
	if (c==' ') lines[i].a++;
	else break;
	}
	//去掉后面的
	for (size_t k=abm.b-1; k>abm.a; k--) {
	c=m_dat[k];
	if (c==' ') lines[i].b--;
	else break;
	}
	}
	*/
}

size_t XStringList::LoadFromString(string src, const char* lineMark, const char* _closeMark)
{
	ParseEx2(src.c_str(), src.length(), lineMark, _closeMark);
	FixABSpace();

#if(0)
	for (size_t i = 0; i < lines.size(); i++)
	{
		string s = at(i);
		cout << i << " => " << s << endl;
	}
	//	getchar();
#endif
	return lines.size();
}



int XStringList::GetFeildLineVauleInt(string keyname)
{
	int r = 0;
	string str = GetFeildLineVauleString(keyname);
	if (!str.empty()) r = strtol(str.c_str(), NULL, 10);
	return r;
}

string XStringList::GetFeildLineVauleString(string keyname)
{
	if (keyname.empty()) return "";

	XStringList sl;
	string s, v, fi;
	//    fi=string(fieldn);
	//    XString::TrimLR(fi);

	//取＝前 和后
	for (size_t i = 0; i<size(); i++) {
		sl.LoadFromString(at(i), "=");
		if (sl[0].compare(keyname) == 0 && sl.size()>1)
		{
			//            memset(strbuf,0,4096);
			//            strncpy(strbuf, v.c_str(), min(4096, (int)v.length()));
			return sl[1];
		}
	}
	return "";
}

//ssize_t XStringList::GetVaulePos(const char* fieldn)
//{
/*
if (fieldn==NULL) return -1;
string s,v,fi;
fi=string(fieldn);
XString::TrimLR(fi);

//取＝前 和后
for (size_t i=0; i<aryString.size(); i++) {
XString::Split4Equal(aryString[i],s,v);
XString::TrimLR(s);
XString::TrimLR(v);
if(s.compare(fi)==0 && v.length()>0){
return i;
break;
}
}
*/
//    return -1;
//}




void XStringList::Test()
{
	const  char* s = { "1\n\r22\r\n333\"44 \r44\"  55555  \r\n666666 7 1 22\0" };
	XStringList strlist;
	strlist.LoadFromString(s);
	strlist.List();

	strlist.FixABSpace();
	strlist.List();

	//    strlist.Parse(s);
	//    for (size_t i=0; i<strlist.aryString.size(); i++) {
	//        printf("%s\n",strlist.aryString[i].c_str());
	//    } 
	//    int r = strlist.GetVaule("1a");
	//    printf("%s,%d\n","1a",r);


}


void XStringList::List()
{
	for (size_t i = 0; i<size(); i++) {
		printf("%zd:(%s)\n", i, this->at(i).c_str());
	}
}

//void XStringList::Add(const char* text)
//{
//    if (text==NULL) return;
//    string str=string(text);
//    aryString.push_back(str);
//}

size_t XStringList::size()
{
	return lines.size();
}

void XStringList::push_back(string str)
{
	//ABM abm;
	//abm.a = m_dat.size();
	//abm.b = str.length()+abm.a;
	//lines.push_back(abm);
	m_dat.append(str);
	lines.push_back(m_dat.length());
}

size_t XStringList::LoadFromFile(const char* fname)
{
	string str, strbuf;
	string fstr = string(fname);
	ifstream infile;
	infile.open(fname, std::ios_base::in);

	//    aryString.clear();

	while (infile.good())
	{
		std::getline(infile, str);
		//str.replace(0, -1, "\r");
		//        unsigned long apos=str.find("\r");
		//        str = str.substr(0,apos);
		//fixme        aryString.push_back(str);
		//		str.append("\r");
		//        strbuf.append(str);
		push_back(str);
	}

	infile.close();

	//ParseEx2(strbuf.c_str(),strbuf.length(),"\r\n",'"');
	//LoadFromString(strbuf);
	return lines.size();
}

string XStringList::operator[](size_t idx)
{
	size_t _s = 0;
	string res;
	//	size_t _ll = 0;
	//	if(m_xendl) _ll= strlen(m_xendl);
	if (idx<size()) {
		if (idx > 0) _s = lines[idx - 1];
		size_t _e = lines[idx];
		res = m_dat.substr(_s, _e - _s);

		//if (m_xendl && strlen(m_xendl)>0) {
		//	int pos = (int)res.find(m_xendl);
		//	if (pos != std::string::npos) {
		//		res.erase(pos);
		//	}
		//}
	}
	return res;
}



string XStringList::at(size_t idx)
{
	return operator[](idx);
}


/////
/*
XTokenizer::XTokenizer(const std::string& _str, const std::string _delim)
{
m_dat = _str;

string _dat = _str;
_dat = TokenClear(_dat, " \r\n");
ParseEx2(_dat.c_str(), _dat.length(), _delim.c_str(), "\"");
}

XTokenizer::XTokenizer(const char* _str, std::size_t sz,const std::string _delim)
{
m_dat.clear();
if (sz < 1) sz = strlen(_str);
m_dat.append(_str,sz);
string _dat = m_dat;
_dat = TokenClear(_dat, " \r\n");
//	printf("%s",_dat.c_str());
ParseEx2(_dat.c_str(), _dat.length(), _delim.c_str(), "\"");
}
*/

/*
size_t XTokenizer::Parse(const char* lpBuf,ssize_t lpBufSize,const char* _spchar, char _close)
{

if (lpBuf == NULL) return 0;
if (_spchar == NULL) _spchar = " ";

token_str.clear();
//    m_dat=string(lpBuf);
m_dat.clear();
m_dat.append(lpBuf,lpBufSize);

string sp=_spchar;
//	size_t p=0;
char c=0;
bool spl=false;
string lsi;

if (lpBufSize==-1) lpBufSize=strlen(lpBuf);

for(ssize_t i=0;i<lpBufSize;i++)
{
c = lpBuf[i];

if (c==_close){

if (lsi.length()>0)
{
token_str.push_back(lsi);
lsi.clear();
}
spl=!spl;
}

if (spl){
if (c!=_close)  lsi.push_back(c);
continue;
}

if ( sp.find(c)!=std::string::npos )
{
if (lsi.length()>0)
{
token_str.push_back(lsi);
lsi.clear();
continue;
}
}else
{
if (c!=_close ) lsi.push_back(c);
}

}

if (lsi.length()>0) token_str.push_back(lsi);

return token_str.size();

}
*/

bool XTokenizer::IsExistToken(const std::string& keyname)
{
	/*
	std::list<string>::iterator it = token_str.begin();
	for (; it != token_str.end();it++)
	{
	if (keyname.compare(*it) == 0) return true;
	}
	*/

	for (size_t i = 0; i<size(); i++) {
		if (keyname.compare(at(i)) == 0) return true;
	}

	return false;
}

const char* XTokenizer::GetTokenValue(std::string keyname)
{
	/*
	std::list<string>::iterator it = token_str.begin();
	for (; it != token_str.end(); it++)
	{
	if (keyname.compare(*it) == 0){
	it++;
	if (it != token_str.end()){
	cur_token = it;
	return (*it).c_str();
	}
	}
	}
	*/
	for (size_t i = 0; i<size(); i++) {
		if (at(i).compare(keyname) == 0 && size()>i) return at(i + 1).c_str();
	}

	return NULL;
}


int XTokenizer::GetTokenValueInt(const std::string& keyname)
{
	const char* s = GetTokenValue(keyname);
	if (s) return atoi(s);
	return 0;
}

int XTokenizer::GetTokenValueInt(unsigned int _pos)
{
	/*
	if (token_str.size() < _pos) return 0;

	std::list<string>::iterator it = token_str.begin();// +_pos;

	for (unsigned int i = 0; i < _pos; i++){
	it++;
	//		printf("%s\n", it->c_str());
	}
	//	if (it == token_str.end()) return 0;
	//printf("==[%s]==\n", it->c_str());
	if (it->length() > 0) return atoi(it->c_str());
	*/

	return atoi(at(_pos).c_str());

}

double XTokenizer::GetTokenValueFloat(const std::string& keyname)
{
	const char* s = GetTokenValue(keyname);
	if (s) return strtod(s, NULL);
	return 0;
}

void XTokenizer::Print()
{
	/*
	std::list<string>::iterator it = token_str.begin();
	for (; it != token_str.end(); it++)
	{
	printf("%s\n",(*it).c_str());
	}
	*/
	for (size_t i = 0; i<size(); i++) {
		printf("%d=%s\n", (int)i, at(i).c_str());
	}
}

bool XTokenizer::FirstTokenIs(const std::string keyname)
{
	if (size()<1) return false;
	//std::list<string>::iterator it = token_str.begin();

	//把前面的空格 末尾的空格 回车符号去掉
	string s = TokenClear(at(0));
	//	printf("==%s==\n",s.c_str());
	return s.compare(keyname) == 0;
}

/*
void ToUpperString(string &str)
{
transform(str.begin(), str.end(), str.begin(), (int (*)(int))toupper);
}
void ToLowerString(string &str)
{
transform(str.begin(), str.end(), str.begin(), (int (*)(int))tolower);
}
*/

inline void strToLower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), (int(*)(int))tolower);
}

bool XTokenizer::FirstTokenIsIcase(const std::string  keyname)
{
	if (size()<1) return false;
	string s = TokenClear(at(0));
	strToLower(s);
	//	std::transform(s.begin(), s.end(), s.begin(), tolower);

	string s1 = keyname;
	strToLower(s1);
	//	std::transform(s1.begin(), s1.end(), s1.begin(), tolower);
	return s.compare(s1) == 0;
}


bool XTokenizer::SecondTokenIs(const std::string keyname)
{
	if (size()<2) return false;
	//std::list<string>::iterator it = token_str.begin();
	//it++;
	//if (it == token_str.end()) return false;
	string s = TokenClear(at(1));
	return s.compare(keyname) == 0;
}




size_t XTokenizer::Parse(const char* buf, size_t sz, const char* sp/*=" \r\n"*/, const char cc/*='"'*/)
{
	if (sz < 1 || buf == NULL) return 0;

	m_dat.clear();
	m_dat.append(buf, sz);

	if (sp == NULL) sp = " \r\n";

	size_t spl = strlen(sp);
	bool bc = false;

	for (size_t i = 0; i < sz; i++)
	{
		char c = buf[i];

		if (c == cc)
		{
			if (bc) {
				lines.push_back(i);
			}
			bc = !bc;
			continue;
		}

		if (bc) continue;

		for (size_t j = 0; j<spl; j++)
		{
			if (sp[j] == c) {
				lines.push_back(i);
				break;
			}
		}
	}

	if (lines.size()>0 && lines.back() != sz)  lines.push_back(sz);

	if (lines.empty()) lines.push_back(sz);

	return lines.size();
}


void XTokenizer::KickNLine(const char* sp)
{
	//	size_t fl = 0;
	//	size_t a=0, b=0;
	size_t spl = strlen(sp);
	bool b = false;
	size_t n = 0;
	size_t l = 0;
	auto it = lines.begin();
	while (it != lines.end())
	{
		b = false;
		n = 0;
		std::string str = GetString(l);
		for (size_t j = 0; j < str.length(); j++)
		{
			char c = str[j];
			for (size_t k = 0; k < spl; k++)
			{
				if (c == sp[k]) {
					b = true;
					break;
				}
			}
			if (b) n++;
		}
		if (str.empty() || n == str.length()) {
			//全是分行符 删除这个行
			it = lines.erase(it);
			continue;
		}
		it++;
		l++;
	}
}

//size_t XTokenizer::Parse3(const char* buf, size_t sz, const char* sp/*=" \r\n"*/, const char cc/*='"'*/)
//{
//	if (sz < 1 || buf == NULL) return 0;
//
//	m_dat.clear();
//	m_dat.append(buf, sz);
//
//	if (sp == NULL) sp = " \r\n";
//
//	size_t spl = strlen(sp);
//	bool bc = false;
//	bool nn = false;
//	size_t cl = 0;
//
//	for (size_t i = 0; i < sz; i++)
//	{
//		char c = buf[i];
//
//		if (c == cc)
//		{
//			if (bc && cl > 0) {
//				lines.push_back(i);
//				cl = 0;
//			}
//			bc = !bc;
//			continue;
//		}
//
//		if (bc) continue;
//
//		for (size_t j = 0; j < spl; j++)
//		{
//			if (sp[j] == c && cl > 0) {
//				//				lines.push_back(i);
//				//				cl = 0;
//				nn = true;
//				break;
//			}
//		}
//
//		if (nn) {
//			lines.push_back(i);
//			cl = 0;
//			nn = false;
//			continue;
//		}
//
//		cl++;
//	}
//
//	if (lines.size() > 0 && lines.back() != sz)  lines.push_back(sz);
//
//	if (lines.empty()) lines.push_back(sz);
//
//	return lines.size();
//}


string XTokenizer::TokenClear(string str, string rstr, int _type)
{
	if (str.length()<1) return "";
	string result = str;
	char sp;
	size_t i = 0;
	//处理前面
	for (; i<str.length(); i++) {
		sp = str[i];
		if (rstr.find(sp) == std::string::npos) break;
	}
	result.erase(0, i);

	if (result.length()<1) return result;

	if (_type == 0) return result;

	//处理后面
	i = result.length() - 1;
	for (; i >= 0; i--) {
		sp = result[i];
		if (rstr.find(sp) == std::string::npos) break;
	}
	i++;
	result.erase(i, result.length() - i);

	return result;
}

//还没测试
std::string XTokenizer::GetTokenRemain(const std::string& keyname)
{
	std::string result;
	string::size_type pos = m_dat.find(keyname);

	if (pos != string::npos) {
		size_t _start = pos + keyname.length();
		size_t _len = m_dat.length() - _start;
		result = m_dat.substr(_start, _len);
	}

	return result;
}

size_t XTokenizer::GetTokenIndex(const string& keyname)
{
	for (size_t i = 0; i < size(); i++) {
		if (at(i).compare(keyname) == 0) return i;
	}
	return 0;
}

std::string XTokenizer::GetTokenValueString(const string& keyname)
{
	string res;
	//const char* s = GetTokenValue(keyname);
	//if (s) res = string(s);
	size_t idx = GetTokenIndex(keyname) + 1;
	if (idx < size()) return this->at(idx);
	return res;
}

int XTokenizer::GetValueIntByIndex(unsigned int _idx)
{
	std::string str = GetValueStringByIndex(_idx);
	if (str.empty()) return 0;
	return atoi(str.c_str());
}

std::string XTokenizer::GetValueStringByIndex(unsigned int _idx)
{
	return TokenClear(at(_idx));
}

std::string XTokenizer::GetString(unsigned int _pos)
{
	/*
	if (token_str.size() < _pos) return 0;
	std::list<string>::iterator it = token_str.begin();// +_pos;

	for (unsigned int i = 0; i < _pos; i++){
	it++;
	//		printf("%s\n", it->c_str());
	}
	*/
	return TokenClear(at(_pos));
}

//void test_XTokenizer()
//{
//	XTokenizer tok("GAME DML replace into xserver () values () ;"," \r\n");
//	string str=tok.GetTokenRemain("DML");
//	printf("%s\n",str.c_str());
//}



////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void XTextParamList::LoadFromAsciiFile(std::string filename)
{

	string str;
	ifstream infile;

	//FIXME 这里要读UTF8文件格式

	//	valueMap.clear();

	infile.open(filename.c_str(), std::ios_base::in);

	while (infile.good())
	{
		std::getline(infile, str);
		ParseLine(str);
	}

	infile.close();
}

void XTextParamList::Parse(const char * _lpBuf, size_t _size)
{
	if (_lpBuf == NULL) return;

	//	valueMap.clear();

	XStringList sl;
	std::string str;
	str.append(_lpBuf, _size);

	sl.LoadFromString(str);
	for (size_t i = 0; i < sl.size(); i++)
	{
		str = sl.at(i);
		ParseLine(str);
	}
}

std::string XTextParamList::GetValue(std::string keyname)
{
	return valueMap[keyname];
}

std::wstring XTextParamList::GetValueWString(std::string keyname)
{
	//std::string s1 = valueMap[keyname];
	return utf8util::s2ws(valueMap[keyname]);
}

bool XTextParamList::IsExist(std::string keyname)
{
	return (valueMap.find(keyname) != valueMap.end());
}

void XTextParamList::Print()
{
	size_t k = 0;
	for (auto it = valueMap.begin(); it != valueMap.end(); it++)
	{
		cout << k << "=>" << it->first << "=[" << it->second << "]" << endl;
		k++;
	}
}

void XTextParamList::LoadFile(const char * filename)
{
	wstring ws1;
	string s1;
	size_t _start = 0;
	bool _writeBOM = false;
	XTextFile txtfile;
	txtfile.LoadFile(filename);
	char* p = (char*)txtfile.c_str();
	p = utf8util::SkipWhiteSpace(p);
	p = const_cast<char*>(utf8util::ReadBOM(p, &_writeBOM));
	if (!*p) {
		//SetError(XML_ERROR_EMPTY_DOCUMENT, 0, 0);
		return;
	}
	_start = p - txtfile.c_str();

	//开始分析
	if (_writeBOM) {
		size_t len = utf8::distance(txtfile.c_str(), txtfile.c_str() + txtfile.size());
		//size_t len = utf8::distance((const char*)p, txtfile.c_str() + txtfile.size()-_start);
		ws1.resize(len);
		if (len)
			utf8::utf8to16(txtfile.c_str(), txtfile.c_str() + txtfile.size(),
				&ws1[0]);
		ws1.erase(0, 1);
		s1 = utf8util::ws2s(ws1);
	}
	else {
		s1.append(txtfile.c_str(), txtfile.size());
	}

	Parse(s1.c_str(), s1.length());

}

void XTextParamList::ParseLine(std::string line)
{
	//第一个字母除空格外如果是 # 直接退出
	/*
	for (size_t i = 0; i < line.length(); i++)
	{
	char ch = line[i];
	if (::isspace(int(ch))) continue;
	if (ch == '#') return;
	break;
	}
	*/
	string ch;
	for (size_t i = 0; i < line.length(); i++)
	{
		ch = line.substr(i, 1);// line[i];
		if (ch.compare(" ") == 0) continue; //if (::isspace(int(ch))) continue;
		if (ch.compare("#") == 0) return;
		break;
	}


	//找到第一个 =
	std::string::size_type pos = line.find('=');
	if (pos == std::string::npos) return;

	std::string name = line.substr(0, pos);
	std::string value = line.substr(pos + 1);

	//去除前后空格
	XStrUtil::TrimSpaceLeftAndRight(name);
	XStrUtil::TrimSpaceLeftAndRight(value);

	if (name.empty() || value.empty()) return;

	valueMap[name] = value;

}

std::string XTextParamList::GetValue(std::string filename, std::string keyname)
{
	XTextParamList tpl;
	tpl.LoadFromAsciiFile(filename);
	return tpl.GetValue(keyname);
}



//////////////////////////////////////////////////////////////////////////
// XTextFile
//////////////////////////////////////////////////////////////////////////

XTextFile::XTextFile()
	:dat(0)
	, len(0)
{
}

XTextFile::~XTextFile()
{
	if (dat) delete[] dat;
}

bool XTextFile::LoadFile(const char * filename)
{
	if (filename == NULL) return false;

	FILE* fp = callfopen(filename, "rb");
	if (!fp) {
		//SetError(XML_ERROR_FILE_NOT_FOUND, filename, 0);
		return false;
	}
	LoadFile(fp);
	fclose(fp);

	return true;
}

int XTextFile::LoadFile(FILE * fp)
{
	fseek(fp, 0, SEEK_SET);
	if (fgetc(fp) == EOF && ferror(fp) != 0) {
		//		SetError(XML_ERROR_FILE_READ_ERROR, 0, 0);
		return -2;
	}

	fseek(fp, 0, SEEK_END);
	const long filelength = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (filelength == -1L) {
		//SetError(XML_ERROR_FILE_READ_ERROR, 0, 0);
		return -3;
	}
	assert(filelength >= 0);

	dat = new uchar[filelength + 1];
	dat[filelength] = 0;

	size_t read = fread(dat, 1, filelength, fp);
	if (read != filelength) {
		//SetError(XML_ERROR_FILE_READ_ERROR, 0, 0);
		return -4;
	}
	len = filelength;

	return 0;
}




namespace XStringUtil
{

	Tokens StrSplit(const std::string& src, const std::string& sep)
	{
		Tokens r;
		std::string s;
		for (std::string::const_iterator i = src.begin(); i != src.end(); ++i)
		{
			if (sep.find(*i) != std::string::npos)
			{
				if (s.length()) r.push_back(s);
				s = "";
			}
			else
			{
				s += *i;
			}
		}
		if (s.length()) r.push_back(s);
		return r;
	}


	uint32 GetUInt32ValueFromArray(Tokens const& data, uint16 index)
	{
		if (index >= data.size())
			return 0;

		return (uint32)atoi(data[index].c_str());
	}

	float GetFloatValueFromArray(Tokens const& data, uint16 index)
	{
		float result;
		uint32 temp = GetUInt32ValueFromArray(data, index);
		memcpy(&result, &temp, sizeof(result));

		return result;
	}

	void stripLineInvisibleChars(std::string& str)
	{
		static std::string invChars = " \t\7\n";

		size_t wpos = 0;

		bool space = false;
		for (size_t pos = 0; pos < str.size(); ++pos)
		{
			if (invChars.find(str[pos]) != std::string::npos)
			{
				if (!space)
				{
					str[wpos++] = ' ';
					space = true;
				}
			}
			else
			{
				if (wpos != pos)
					str[wpos++] = str[pos];
				else
					++wpos;
				space = false;
			}
		}

		if (wpos < str.size())
			str.erase(wpos, str.size());
	}

	std::string secsToTimeString(time_t timeInSecs, bool shortText, bool hoursOnly)
	{
		time_t secs = timeInSecs % MINUTE;
		time_t minutes = timeInSecs % HOUR / MINUTE;
		time_t hours = timeInSecs % DAY / HOUR;
		time_t days = timeInSecs / DAY;

		std::ostringstream ss;
		if (days)
			ss << days << (shortText ? "d" : " Day(s) ");
		if (hours || hoursOnly)
			ss << hours << (shortText ? "h" : " Hour(s) ");
		if (!hoursOnly)
		{
			if (minutes)
				ss << minutes << (shortText ? "m" : " Minute(s) ");
			if (secs || (!days && !hours && !minutes))
				ss << secs << (shortText ? "s" : " Second(s).");
		}

		return ss.str();
	}

	uint32 TimeStringToSecs(const std::string& timestring)
	{
		uint32 secs = 0;
		uint32 buffer = 0;
		uint32 multiplier = 0;

		for (std::string::const_iterator itr = timestring.begin(); itr != timestring.end(); ++itr)
		{
			if (isdigit(*itr))
			{
				buffer *= 10;
				buffer += (*itr) - '0';
			}
			else
			{
				switch (*itr)
				{
				case 'd': multiplier = DAY;     break;
				case 'h': multiplier = HOUR;    break;
				case 'm': multiplier = MINUTE;  break;
				case 's': multiplier = 1;       break;
				default: return 0;                         // bad format
				}
				buffer *= multiplier;
				secs += buffer;
				buffer = 0;
			}
		}

		return secs;
	}

	std::string TimeToTimestampStr(time_t t)
	{
		struct tm tmBuf;
		//	time_t t = time(NULL);

#ifdef _MSC_VER
		//	tm* aTm = localtime(&t);
		localtime_s(&tmBuf, &t);
#else
		localtime_r(&t, &tmBuf);
#endif

		//       YYYY   year
		//       MM     month (2 digits 01-12)
		//       DD     day (2 digits 01-31)
		//       HH     hour (2 digits 00-23)
		//       MM     minutes (2 digits 00-59)
		//       SS     seconds (2 digits 00-59)
		char buf[20];
		snprintf(buf, 20, "%04d-%02d-%02d_%02d-%02d-%02d",
			tmBuf.tm_year + 1900, tmBuf.tm_mon + 1, tmBuf.tm_mday,
			tmBuf.tm_hour, tmBuf.tm_min, tmBuf.tm_sec);

		return std::string(buf);
	}


	size_t utf8length(std::string& utf8str)
	{
		try
		{
			return utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size());
		}
		catch (std::exception)
		{
			utf8str = "";
			return 0;
		}
	}

	void utf8truncate(std::string& utf8str, size_t len)
	{
		try
		{
			size_t wlen = utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size());
			if (wlen <= len)
				return;

			std::wstring wstr;
			wstr.resize(wlen);
			utf8::utf8to16(utf8str.c_str(), utf8str.c_str() + utf8str.size(), &wstr[0]);
			wstr.resize(len);
			char* oend = utf8::utf16to8(wstr.c_str(), wstr.c_str() + wstr.size(), &utf8str[0]);
			utf8str.resize(oend - (&utf8str[0]));               // remove unused tail
		}
		catch (std::exception)
		{
			utf8str = "";
		}
	}

	bool Utf8toWStr(char const* utf8str, size_t csize, wchar_t* wstr, size_t& wsize)
	{
		try
		{
			size_t len = utf8::distance(utf8str, utf8str + csize);
			if (len > wsize)
			{
				if (wsize > 0)
					wstr[0] = L'\0';
				wsize = 0;
				return false;
			}

			wsize = len;
			utf8::utf8to16(utf8str, utf8str + csize, wstr);
			wstr[len] = L'\0';
		}
		catch (std::exception)
		{
			if (wsize > 0)
				wstr[0] = L'\0';
			wsize = 0;
			return false;
		}

		return true;
	}

	bool Utf8toWStr(const std::string& utf8str, std::wstring& wstr)
	{
		try
		{
			size_t len = utf8::distance(utf8str.c_str(), utf8str.c_str() + utf8str.size());
			wstr.resize(len);

			if (len)
				utf8::utf8to16(utf8str.c_str(), utf8str.c_str() + utf8str.size(), &wstr[0]);
		}
		catch (std::exception)
		{
			wstr = L"";
			return false;
		}

		return true;
	}

	bool WStrToUtf8(wchar_t* wstr, size_t size, std::string& utf8str)
	{
		try
		{
			std::string utf8str2;
			utf8str2.resize(size * 4);                          // allocate for most long case

			char* oend = utf8::utf16to8(wstr, wstr + size, &utf8str2[0]);
			utf8str2.resize(oend - (&utf8str2[0]));             // remove unused tail
			utf8str = utf8str2;
		}
		catch (std::exception)
		{
			utf8str = "";
			return false;
		}

		return true;
	}

	bool WStrToUtf8(std::wstring wstr, std::string& utf8str)
	{
		try
		{
			std::string utf8str2;
			utf8str2.resize(wstr.size() * 4);                   // allocate for most long case

			char* oend = utf8::utf16to8(wstr.c_str(), wstr.c_str() + wstr.size(), &utf8str2[0]);
			utf8str2.resize(oend - (&utf8str2[0]));             // remove unused tail
			utf8str = utf8str2;
		}
		catch (std::exception)
		{
			utf8str = "";
			return false;
		}

		return true;
	}


	bool Utf8FitTo(const std::string& str, std::wstring search)
	{
		std::wstring temp;

		if (!Utf8toWStr(str, temp))
			return false;

		// converting to lower case
		wstrToLower(temp);

		if (temp.find(search) == std::wstring::npos)
			return false;

		return true;
	}


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
		for (const char* s = &strSrc[pos + name.length()]; s != _endstr; s++)
		{
			if ((s[0] == ':' || s[0] == '=') && _start == false) continue;
			if ((s[0] == ' ' || s[0] == '&' || s[0] == '\n') && _start) break;
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
		char c = 0;
		do
		{
			pos = str.find("&#", pos);
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
			pos = str.find(o_str, pos);
			if (pos != std::string::npos) {
				str.erase(pos, olen);
				if (nlen>0) str.insert(pos, n_str);
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

	bool GetValueStringFromURL(std::string& result, std::string url, std::string kname)
	{
		using namespace std;
		string::size_type pos = 0, posR = -1;

		if (kname.empty() || url.empty()) return false;
		std::string _url = url;// = DecodeURL(url);

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
		while (!result.empty())
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


XLanguage* XLanguage::_instance = NULL;

XLanguage* XLanguage::Instance()
{
	if (_instance == NULL) _instance = new XLanguage();
	return _instance;
}
