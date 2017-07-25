//
//  XHttp.cpp
//  XNetGame
//
//  Created by im on 12-11-15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#pragma warning(disable:4996)

//#include "curl/curl.h"
//#include "gayola.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>

//#include "XTcpClient.h"
//#include "XString.h"
#include "XHttp3.h"
//#include "XByteBuffer.h"
//#include "XDataStorage.h"

//#include "XFileSystem.h"


//#include <winsock2.h>
//#include <ws2tcpip.h>
//#pragma comment (lib, "Ws2_32.lib")

#define XSAFE_FREE(a) if(a) free(a); a=NULL;

//int hostname_to_ip(const char *hostname, char *ip, const char* port = "http");

using namespace std;



HttpResponse::HttpResponse()
{
//	m_Data = NULL;
	len_Data = 0;
	rpos_Data = 0;

}

HttpResponse::~HttpResponse()
{
//	XSAFE_FREE(m_Data);
}


/*
HttpResponse::HttpResponse(string& _data)
{
	strData = _data;
	Parse();
}
*/

HttpResponse& HttpResponse::operator<< (const string& value)
{
	Append(value.c_str(), value.length());
	return *this;
}

HttpResponse& HttpResponse::operator<< (const char* value)
{
	Append(value, strlen(value));
	return *this;
}

HttpResponse& HttpResponse::operator<< (const unsigned char* value)
{
	size_t ilen = strlen((char*)value);
	Append(value, ilen);
	return *this;
}

int HttpResponse::Append(const void* value, size_t sz)
{
	m_bbf.append((const uint8*)value, sz);

	

	return 0;

	//这里增加需要的长度
	//CheckDataSize(sz);
	//memcpy(m_Data + rpos_Data, value, sz);
	//rpos_Data += sz;
}

void HttpResponse::CheckDataSize(size_t sz)
{
/*	
	if (m_Data == NULL) {
		m_Data = (unsigned char*)malloc(sz>40960?sz:40960);
		len_Data = 40960;
	}

	int ri = 0 - len_Data - rpos_Data - sz;
	
	if ( ri> 0 ) {
		m_Data = (unsigned char*)realloc(m_Data, len_Data + ri);
		len_Data += ri;
	}
*/
}

//void HttpResponse::Parse()
//{
//	//\r\n\r\n 内容
//	int pos = strData.find("\r\n\r\n");
//	if (pos != string::npos) {
//		iContentLength = strData.length() - pos-4;
//		strContent = strData.substr(pos+4, iContentLength);
//	}
//}

//string& HttpResponse::Content()
//{
//	return strContent;
//}

/*
void HttpResponse::Parse()
{

	//查询内容长度
	string strHead;

	//\r\n\r\n 内容
	const char* _dat = strstr((const char*)m_DataBbf.contents(),"\r\n\r\n");// strData.find("\r\n\r\n");


	ihead_len = _dat - (char*)m_DataBbf.contents();
	char* hb = (char*)malloc(ihead_len);

	m_DataBbf.read((uint8*)hb, ihead_len);
	//memcpy(hb, m_DataBbf.contents(), ihead_len);
	strHead = string(hb);
	free(hb);

//分析头部
	XStringList slist;
	slist.Parse(strHead.c_str());
	for (int i = 0; i < slist.GetNumLine(); i++)
	{
		string _s=slist[i];
		int ip= _s.find(":");
		string sfi = _s.substr(0, ip);
		string sfv = _s.substr(ip + 1, _s.length() - ip + 1);
		mapHead[sfi] = sfv;
	}

	ptrContent =_dat+ 4;
	
	if(mapHead.find("Content-Length")!=mapHead.end())
		iContentLength = atoi(mapHead["Content-Length"].c_str());
	else iContentLength = m_DataBbf.wpos() - ihead_len-4;

}
*/


/*
Chunked编码使用若干个Chunk串连而成，由一个标明长度为0的chunk标示结束。
每个Chunk分为头部和正文两部分，头部内容指定下一段正文的字符总数（十六进制的数字）和数量单位（一般不写），
正文部分就是指定长度的实际内容，两部分之间用回车换行(CRLF)隔开。
在最后一个长度为0的Chunk中的内容是称为footer的内容，是一些附加的Header信息（通常可以直接忽略）。

*/

void HttpResponse::ParseChunk()
{
//	char*   pdata = (char*)m_Data;
	char*   pdata = (char*)m_bbf.contents();
//	char*   pdataend = pdata + rpos_Data;
	char*   pdataend = pdata + m_bbf.size();
	char*   pchunkflag = strstr(pdata, "Transfer-Encoding: chunked");
	if (!pchunkflag)    return;

//	string input = string(pdata);

	char*   pStart = strstr(pdata, "\r\n\r\n");
//	string result;
	if (pStart && (pStart += 4) < pdataend)
	{
		char*   pEnd = 0;
		do {
			pEnd = strstr(pStart, "\r\n");
			if (pEnd)
			{
				string hexsize(pStart);
				hexsize = hexsize.substr(0, pEnd - pStart);
				char* str;
				long lSize = strtol(hexsize.c_str(), &str, 16); //转长度
				if (!lSize) break;
				if (pEnd + 2 + lSize < pdataend)
				{
					pEnd += 2;
					//result.append(input.substr(pEnd - pStart, lSize));
					//6 的长度 2255\r\n
					m_Content.append(pEnd, lSize);
					pStart = pEnd + lSize+2; 
				}
				else {
					break;
				}
			}
			else { break; }
		} while (pStart && pEnd);
	}
//	return result;
//	m_Content.append(result.c_str(),result.length());

	iContentLength = m_Content.size();

}



void HttpResponse::ParseContentForChunked()
{
//	const char* _dat = (const char*)m_Data + ihead_len + 4;
	//这里开始处理
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
		if(std::string::npos==estr.find(c)) break;
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



void HttpResponse::Parse()
{
	string str;

	if (m_bbf.size() == 0) return;

	//协议头部数据
	string strHead;

	//\r\n\r\n 内容
//	const char* _dat = strstr((const char*)m_Data, "\r\n\r\n");// strData.find("\r\n\r\n");
	const char* _dat = strstr((const char*)m_bbf.contents(), "\r\n\r\n");// strData.find("\r\n\r\n");
	
	if (_dat == NULL) return;
	
	const char* _dStart = (const char*)m_bbf.contents();

	

	ihead_len = _dat - (char*)m_bbf.contents();
	//char* hb = (char*)malloc(ihead_len+1);
	//memset(hb, 0, ihead_len + 1);
	//memcpy(hb, _dStart, ihead_len);
	//strHead = string(hb);
	//free(hb);

	strHead.clear();
	strHead.append(_dStart, ihead_len);


	//分析头部
	//XStringList slist;
	//slist.ParseEx2(strHead.c_str());

	//解析第一行
	const char* _ep = strstr(_dStart, "\r");
	if (_ep)
	{
		str.clear();
		str.append(_dStart, _ep - _dStart);

		//去掉最前面的协议标识
		int p = str.find(" ");
		PVer = str.substr(0, p);

		//中间返回编号
		int p1 = str.find(" ",p+1);
		std::string s1 = str.substr(p + 1, p1 - p - 1);
		resultCode = std::strtod(s1.c_str(), NULL);

		//最后面的错误提示
		ErrorString = str.substr(p1 + 1);
	}

	//string sfirst = slist[0];
	//XStringList slf;
	//slf.ParseEx2(sfirst.c_str(), sfirst.length(), " ");
	//resultCode = atoi(slf[1].c_str());

#if(0)
	for (size_t i = 1; i < slist.size(); i++)
	{
		string _s = slist[i];
		int ip = _s.find(":");
		string sfi = _s.substr(0, ip);
		string sfv = _s.substr(ip + 1, _s.length() - ip + 1);
		//这里考虑将前面的空字符去掉
		mapHead[sfi] = sfv;
	}
#else

	//一行一行分析
	const char* _epB=_ep+1;
	const char* _epE=_dStart+ ihead_len;
	do 
	{
		_ep=strstr(_epB, "\r");

		str.clear();
		str.append(_epB, _ep - _epB);

		int ip = str.find(":");
		string sfi = str.substr(0, ip);

		//去掉前面空格和换行符
		//while(1)
		//{
		//	char c = sfi.front();
		//	if (c == ' ' || c == '\n' || c == '\r') sfi.erase(sfi.begin());
		//	else break;
		//}
		trimString(sfi, "\r\n ");

		std::transform(sfi.begin(), sfi.end(),sfi.begin(), tolower);

		string sfv = str.substr(ip + 1, str.length() - ip + 1);

		trimString(sfv, "\r\n ");
		std::transform(sfv.begin(), sfv.end(), sfv.begin(), tolower);

		//这里考虑将前面的空字符去掉
		mapHead[sfi] = sfv;


		_epB = _ep + 1;

	} while (_ep<_epE);


#endif



	//ptrContent = _dat + 4;

	//如果没发现长度标记 会导致png文件多接收到13个字节 原因未明
	std::string kvalue=GetHeadFeildValueIgnoreCase("transfer-encoding");
	//if (mapHead["transfer-encoding"].compare("chunked") == 0)
	if(kvalue.compare("chunked")==0)
	{
		//ParseContentForChunked();
		ParseChunk();
		return;
	}

	kvalue = GetHeadFeildValueIgnoreCase("content-length");
	//if (mapHead.find("content-length") != mapHead.end())
	if(!kvalue.empty())
		iContentLength = atoi(kvalue.c_str());
	else iContentLength = rpos_Data - ihead_len - 4;

	if (iContentLength>0) m_Content.append(_dat + 4, iContentLength);

}


#if(0)
void HTTPSocket::OnRawData(const char *buf, size_t len)
{
	if (!m_header)
	{
		if (m_b_chunked)
		{
			size_t ptr = 0;
			while (ptr < len)
			{
				switch (m_chunk_state)
				{
				case 4:
					while (ptr < len && (m_chunk_line.size() < 2 || m_chunk_line.substr(m_chunk_line.size() - 2) != "\r\n"))
						m_chunk_line += buf[ptr++];
					if (m_chunk_line.size() > 1 && m_chunk_line.substr(m_chunk_line.size() - 2) == "\r\n")
					{
						OnDataComplete();
						// prepare for next request(or response)
						m_b_chunked = false;
						SetLineProtocol(true);
						m_first = true;
						m_header = true;
						m_body_size_left = 0;
						if (len - ptr > 0)
						{
							char tmp[TCP_BUFSIZE_READ];
							memcpy(tmp, buf + ptr, len - ptr);
							tmp[len - ptr] = 0;
							OnRead(tmp, len - ptr);
							ptr = len;
						}
					}
					break;
				case 0:
					while (ptr < len && (m_chunk_line.size() < 2 || m_chunk_line.substr(m_chunk_line.size() - 2) != "\r\n"))
						m_chunk_line += buf[ptr++];
					if (m_chunk_line.size() > 1 && m_chunk_line.substr(m_chunk_line.size() - 2) == "\r\n")
					{
						m_chunk_line.resize(m_chunk_line.size() - 2);
						Parse pa(m_chunk_line, ";");
						std::string size_str = pa.getword();
						m_chunk_size = Utility::hex2unsigned(size_str);
						if (!m_chunk_size)
						{
							m_chunk_state = 4;
							m_chunk_line = "";
						}
						else
						{
							m_chunk_state = 1;
							m_chunk_line = "";
						}
					}
					break;
				case 1:
				{
					size_t left = len - ptr;
					size_t sz = m_chunk_size < left ? m_chunk_size : left;
					OnData(buf + ptr, sz);
					m_chunk_size -= sz;
					ptr += sz;
					if (!m_chunk_size)
					{
						m_chunk_state = 2;
					}
				}
				break;
				case 2: // skip CR
					ptr++;
					m_chunk_state = 3;
					break;
				case 3: // skip LF
					ptr++;
					m_chunk_state = 0;
					break;
				}
			}
		}
		else
			if (!m_b_http_1_1 || !m_b_keepalive)
			{
				OnData(buf, len);
				/*
				request is HTTP/1.0 _or_ HTTP/1.1 and not keep-alive

				This means we destroy the connection after the response has been delivered,
				hence no need to reset all internal state variables for a new incoming
				request.
				*/
				m_body_size_left -= len;
				if (!m_body_size_left)
				{
					OnDataComplete();
				}
			}
			else
			{
				size_t sz = m_body_size_left < len ? m_body_size_left : len;
				OnData(buf, sz);
				m_body_size_left -= sz;
				if (!m_body_size_left)
				{
					OnDataComplete();
					// prepare for next request(or response)
					SetLineProtocol(true);
					m_first = true;
					m_header = true;
					m_body_size_left = 0;
					if (len - sz > 0)
					{
						char tmp[TCP_BUFSIZE_READ];
						memcpy(tmp, buf + sz, len - sz);
						tmp[len - sz] = 0;
						OnRead(tmp, len - sz);
					}
				}
			}
	}
}

#endif


const char* HttpResponse::GetContent()
{
	return (char*)m_Content.contents();
	//return (char*)m_Data + ihead_len+4;
	//return (char*)m_Data + ihead_len;
}

std::string HttpResponse::GetHeadFeildValueIgnoreCase(std::string kname)
{
	std::string res;
	std::transform(kname.begin(), kname.end(), kname.begin(), tolower);
	for (auto it : mapHead)
	{
		std::string s1 = it.first;
		std::transform(s1.begin(), s1.end(), s1.begin(), tolower);
		if (s1.compare(kname) == 0)
		{
			res = it.second;
			std::transform(res.begin(), res.end(), res.begin(), tolower);
		}
	}
	return res;
}

//////////////////////////////////////////////////////////////

void HttpRequest::AddResponseHeader(const char* feild, const char* value)
{
	strHead.append(feild);
	strHead.append(": ");
	strHead.append(value);
	strHead.append("\r\n");
}


void HttpRequest::AddResponseHeader(const string& feild, const string& value)
{
	strHead.append(feild);
	strHead.append(": ");
	strHead.append(value);
	strHead.append("\r\n");
}



///////////////////////////////////////////////////
/*
服务器不存在返回 -1
其他返回http服务器的返回码
*/
//int XHtpGet3(const char* url, ByteBuffer* pBBuffer, bool ssl_on, double* _len)
//{
////	std::string rdata;
//	Uri _uri=string(url);
//	std::string protocol = _uri.Protocol();
//	const char* port = "80";
//	int rnum = 0;
//	unsigned char buf[40960];
//	HttpResponse response;
//
//	if (protocol.compare("https") == 0) port = "443";
//
//	XSOCKET cli = tcp_connect(_uri.Host().c_str(), port);
//	if (cli == 0) return -1;
//
//	//创建http请求头
//	std::string msg;
//	msg ="GET "+_uri.Path()+" HTTP/1.0 \n" 
//		+ "Host: "+_uri.Host()+"\n"
//		+"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.0; zh-CN; rv:1.9.1.13) Gecko/20100914 Firefox/3.5.13 ( .NET CLR 3.5.30729) \r\n\r\n";
//
//	tcp_send(cli, msg.c_str(), msg.length());
//	while(1) {
//		memset(buf, 0, sizeof(buf));
//		rnum = tcp_recv(cli, (char*)buf, 40960);
//		if (rnum>0) response<<buf;
//		if (rnum < 1) break;
//	} 
//
//	response.Parse();
//
//	if (pBBuffer) {
//		if (response.iContentLength>0) pBBuffer->append(response.GetContent(),response.iContentLength);
//	}
//
//	return response.resultCode;
//}

//////////////////////////////////

HttpRequest::HttpRequest()
{
	m_http_version = "HTTP/1.1";
	m_method = "GET";
}

HttpRequest::~HttpRequest()
{
	if (m_socket != X_INVALID_SOCKET) xnet_disconnect(m_socket);
}

void HttpRequest::SetMethod(const string& method/* = "GET"*/)
{
	m_method = method;
}

void HttpRequest::SetHttpVersion(const string& ver/* = "HTTP/1.1"*/)
{
	m_http_version = ver;
}

void HttpRequest::SetSocket(XSOCKET s)
{
	m_socket = s;
}

void HttpRequest::Send(const string& dat)
{
	xnet_send(m_socket, dat.c_str(), dat.length());
}

void HttpRequest::SendBuf(const char* lpBuf, size_t sz)
{
	xnet_send(m_socket, lpBuf, sz);
}

void HttpRequest::SendFile(const string& fname)
{
	FILE* _fh = 0;
	
	//fopen_s(&_fh, fname.c_str(), "r+b");
	_fh = fopen(fname.c_str(), "r+b");

	if (_fh) {
		fseek(_fh, 0, SEEK_END);
		long wdata_len = ftell(_fh);
		char* _fdat = (char*)malloc(wdata_len);
		fseek(_fh, 0, SEEK_SET);
		//fread_s(_fdat, wdata_len, wdata_len, 1, _fh);
		fread(_fdat, wdata_len, 1, _fh);
		//wdata.append(_fdat, wdata_len);
		SendBuf(_fdat, wdata_len);
		free(_fdat);
		_fdat = NULL;
		fclose(_fh);
	}
}

void HttpRequest::SetURL(const string& url)
{
	m_uri_original = url;

	m_uri = Zx::Uri::parse(url);

	//std::string protocol = _uri.Protocol();
	//m_port = "80";
	//if (_uri.isSecure()) m_port = "443";

	//m_url = _uri.getPath();
	//m_host = _uri.getHost();

}

void HttpRequest::SendRequest()
{
	std::string msg;
	msg = m_method + " " + m_uri.getPathEtc() + " " + m_http_version + "\r\n";
	msg += strHead;

	for (auto it : m_HeadFeilds)
	{
		if(it.second.empty()) continue;
		string str;
		str = it.first + " : " + it.second + "\r\n";
		msg += str;
	}

	msg += "\r\n";
	Send(msg);
}



void HttpRequest::DoMultipartPost()
{
	long length = 0; // calculate content_length of our post body
	std::string tmp;


	// fields
	{
		for (std::map<std::string, std::list<std::string> >::iterator it = m_fields.begin(); it != m_fields.end(); ++it)
		{
			std::string name = (*it).first;
			std::list<std::string>& ref = (*it).second;
			tmp = "--" + m_boundary + "\r\n"
				"content-disposition: form-data; name=\"" + name + "\"\r\n"
				"\r\n";
			for (std::list<std::string>::iterator it = ref.begin(); it != ref.end(); ++it)
			{
				std::string value = *it;
				tmp += value + "\r\n";
			}
			length += (long)tmp.size();
		}
	}

	// files
	{
		for (std::map<std::string, std::string>::iterator it = m_files.begin(); it != m_files.end(); ++it)
		{
			std::string name = (*it).first;
			std::string filename = (*it).second;
			long content_length = m_content_length[filename];
			std::string content_type = m_content_type[filename];
			tmp = "--" + m_boundary + "\r\n"
				"content-disposition: form-data; name=\"" + name + "\"; filename=\"" + filename + "\"\r\n"
				"content-type: " + content_type + "\r\n"
				"\r\n";
			length += (long)tmp.size();
			length += content_length;
			length += 2; // crlf after file
		}
	}

	// end
	tmp = "--" + m_boundary + "--\r\n";
	length += (long)tmp.size();

	// build header, send body
	SetMethod("POST");
	SetHttpVersion("HTTP/1.1");
	AddResponseHeader("Host", m_host); // oops - this is actually a request header that we're adding..
	AddResponseHeader("User-agent", "Mozilla/5.0 (Windows; U; Windows NT 6.0; zh-CN; rv:1.9.1.13) Gecko/20100914 Firefox/3.5.13");
	AddResponseHeader("Accept", "text/html, text/plain, */*;q=0.01");
	AddResponseHeader("Connection", "close");
	AddResponseHeader("Content-type", "multipart/form-data; boundary=" + m_boundary);
	AddResponseHeader("Content-length", std::to_string(length));

	SendRequest();

	// send fields
	{
		for (std::map<std::string, std::list<std::string> >::iterator it = m_fields.begin(); it != m_fields.end(); ++it)
		{
			std::string name = (*it).first;
			std::list<std::string>& ref = (*it).second;
			tmp = "--" + m_boundary + "\r\n"
				"content-disposition: form-data; name=\"" + name + "\"\r\n"
				"\r\n";
			for (std::list<std::string>::iterator it = ref.begin(); it != ref.end(); ++it)
			{
				std::string value = *it;
				tmp += value + "\r\n";
			}
			Send(tmp);
		}
	}

	// send files
	{
		for (std::map<std::string, std::string>::iterator it = m_files.begin(); it != m_files.end(); ++it)
		{
			std::string name = (*it).first;
			std::string filename = (*it).second;
			std::string content_type = m_content_type[filename];
			tmp = "--" + m_boundary + "\r\n"
				"content-disposition: form-data; name=\"" + name + "\"; filename=\"" + filename + "\"\r\n"
				"content-type: " + content_type + "\r\n"
				"\r\n";
			Send(tmp);
			{
				/*
				std::auto_ptr<IFile> fil = std::auto_ptr<IFile>(new File);
				if (fil->fopen(filename, "rb"))
				{
					char slask[2000]; // for fread
					size_t n;
					while ((n = fil->fread(slask, 1, 2000)) > 0)
					{
						SendBuf(slask, n);
					}
					fil->fclose();
				}*/
				SendFile(filename);
			}
			Send("\r\n");
		}
	}

	// end of send
	Send("--" + m_boundary + "--\r\n");
}

//void HttpRequest::AddFile(const std::string& name, const std::string& filename, const std::string& type)
//{
//	{
//		m_files[name] = filename;
//		m_content_length[filename] = xfs_file_lenght(filename.c_str());
//		m_content_type[filename] = type;
//		m_bMultipart = true;
//	}
//}


void HttpRequest::AddField(const std::string& name, const std::string& value)
{
	std::list<std::string> vec;
	vec.push_back(value);
	AddMultilineField(name, vec);
}

void HttpRequest::AddMultilineField(const std::string& name, std::list<std::string>& values)
{
	m_fields[name] = values;
}

void HttpRequest::GetURL(string surl, HttpResponse* _response)
{
	SetURL(surl);
	m_socket = xnet_connect(m_host.c_str(), std::strtod(m_port.c_str(),0));
	if (m_socket == 0) return;

	SetMethod("GET");
	SetHttpVersion("HTTP/1.1");
	AddResponseHeader("Host", m_host); // oops - this is actually a request header that we're adding..
	AddResponseHeader("User-agent", "Mozilla/5.0");
	AddResponseHeader("Accept", "text/html,image/gif, image/x-xbitmap, image/jpeg,image/png, text/plain, */*;");
	AddResponseHeader("Connection", "close");
	AddResponseHeader("Accept-Encoding", "gzip, deflate");

	SendRequest();
	size_t rrnum = 0;
	if (_response)
	{
		unsigned char buf[40960];
		int rnum = 0;
		while (1) {
			memset(buf, 0, sizeof(buf));
			rnum = xnet_recv(m_socket, (char*)buf, 40960,0);
			if (rnum > 0) { 
				_response->Append(buf, rnum);
				rrnum += rnum;
			}
			if (rnum < 1) break;
		}

		_response->Parse();
	}

}

HttpResponse* HttpRequest::GetDocAfterConnected()
{
	if (m_socket == 0) return NULL;

	SetMethod("GET");
	SetHttpVersion("HTTP/1.1");
	AddResponseHeader("Host", m_host); // oops - this is actually a request header that we're adding..
	AddResponseHeader("User-agent", "Mozilla/5.0");
	AddResponseHeader("Accept", "text/html,image/gif, image/x-xbitmap, image/jpeg,image/png, text/plain, */*;");
	AddResponseHeader("Connection", "close");
	AddResponseHeader("Accept-Encoding", "gzip, deflate");

	SendRequest();
	size_t rrnum = 0;
	HttpResponse* _response = new HttpResponse();
	std::string head;
	size_t cnt_len = 0;

	if (_response)
	{
		unsigned char buf[40960];
		int rnum = 0;

		//先收内容头部
		while (1) {
			rnum = xnet_recv(m_socket, (char*)buf, 1, 0);
			if (rnum > 0) {
				head.push_back(buf[0]);
				rrnum += rnum;
			}
			if (rnum < 1) {
				_response->Append(head.c_str(), head.length());
				goto S1;
			}

			//这里查询是否已经收全了本次内容
			if (cnt_len == 0) {
				//FIXME 这里要不区分大小写
				int pos = head.find("Content-Length");
				int pos1=-1;
				if (pos != std::string::npos) {
					pos += strlen("Content-Length")+1;
					pos1= head.find("\r", pos);
				}
				if (pos != std::string::npos && pos1 > pos)
				{
					std::string str_cnt_len = head.substr(pos, pos1 - pos);
					cnt_len = std::strtol(str_cnt_len.c_str(), NULL, 10);
				}
			}

			if (head.find("\r\n\r\n")!=std::string::npos) break;
		}

		_response->Append(head.c_str(), head.length());

		size_t remain = 40960;
		if (cnt_len > 0) remain = min(cnt_len, 40960);
		rrnum = 0;
		while (1) {
//			memset(buf, 0, sizeof(buf));
			rnum = xnet_recv(m_socket, (char*)buf, remain, 0);
			if (rnum > 0) {
				_response->Append(buf, rnum);
				rrnum += rnum;

				if (cnt_len > 0) {
					remain = cnt_len - rrnum;
					
					if (remain == 0)  //本次内容读完了
					{
						goto S1;
					}
					remain = min(remain, 40960);
				}
			}
			if (rnum < 1) break;

			//这里查询是否已经收全了本次内容

		}
S1:
		_response->Parse();
	}

	//这里有分部分传输过来的数据

/*
编码使用若干个Chunk组成，由一个标明长度为0的chunk结束，每个Chunk有两部分组成，第一部分是该Chunk的长度和长度单位（一般不写），第二部分就是指定长度的内容，每个部分用CRLF隔开。在最后一个长度为0的Chunk中的内容是称为footer的内容，是一些没有写的头部内容。
*/



	return _response;
}

void HttpRequest::SetCookie(std::string _cookie)
{
	m_HeadFeilds["Cookie"]=_cookie;
}

void HttpRequest::Perform(HttpResponse* _response)
{
	m_socket = xnet_connect(m_host.c_str(), std::strtod(m_port.c_str(), 0));
	if (m_socket == 0) return;

//	if(m_method.compare("POST")==0) 
		DoMultipartPost();

//	if (m_method.compare("GET") == 0) SendRequest();

	if (_response)
	{
		char buf[4096];
		int rnum = 0;
			while (1) {
				rnum = xnet_recv(m_socket, buf, 4096,0);
				if (rnum>0)  _response->Append(buf, rnum);
				if (rnum < 1) break;
			}
		
			_response->Parse();
	}

}


///*
//POST报文头如下：
//POST /sn/index.php HTTP/1.1
//Accept: */*
//Accept - Language: zh - cn
//host : localhost
//	Content - Type : application / x - www - form - urlencoded
//	Content - Length : 12
//	Connection : close
//	sn = 123 & n = asa
//	在http头后边有一空行，空行后边接着发送post数据，长度通过Content - Length : 12
//	指出，此post数据中包含两项
//	sn = 123
//	n = asa
//	其中：Content - Type : application / x - www - form - urlencoded 指定POST数据的编码类型
//	Content - Length : 12 POST数据的长度
//
//*/
//int XHtpPost3(const char* url, const char* fname)
//{
//	HttpResponse response;
//	HttpRequest request;
//	request.SetURL(url);
//
//	request.AddField("filename", fname);
//	request.AddFile("userfile", fname, "text/plain");
//
//	request.Perform(&response);
//
////	string wdata;
////	Uri _uri = string(url);
////	std::string protocol = _uri.Protocol();
////	const char* port = "80";
////	int rnum = 0;
////	char buf[4096];
////	HttpResponse response;
////
////	long wdata_len = 0;
////	char* _fdat=NULL;
////
////	char lbuf[256];
////	FILE* _fh = 0;
////	fopen_s(&_fh, fname, "r+b");
////	if (_fh) {
////		fseek(_fh,0, SEEK_END);
////		wdata_len = ftell(_fh);
////		_fdat = (char*)malloc(wdata_len);
////		fseek(_fh, 0, SEEK_SET);
////		fread_s(_fdat, wdata_len, wdata_len, 1, _fh);
////		wdata.append(_fdat, wdata_len);
////		free(_fdat);
////		_fdat = NULL;
////	}
////
////	
////
////	if (protocol.compare("https") == 0) port = "443";
////
////	SOCKET cli = tcp_connect(_uri.Host().c_str(), port);
////	if (cli == 0) return -1;
////
////
////	long length = 0; // calculate content_length of our post body
////	std::string tmp;
////
////	// fields
////	{
////		for (std::map<std::string, std::list<std::string> >::iterator it = m_fields.begin(); it != m_fields.end(); ++it)
////		{
////			std::string name = (*it).first;
////			std::list<std::string>& ref = (*it).second;
////			tmp = "--" + m_boundary + "\r\n"
////				"content-disposition: form-data; name=\"" + name + "\"\r\n"
////				"\r\n";
////			for (std::list<std::string>::iterator it = ref.begin(); it != ref.end(); ++it)
////			{
////				std::string value = *it;
////				tmp += value + "\r\n";
////			}
////			length += (long)tmp.size();
////		}
////	}
////
////
////
////
////	string body;
//////	body += "%0d%0a"; // CRLF
////	body += XString::rfc1738_encode(wdata);
////	
////
////	sprintf(lbuf, "%ld", body.length());
////
////	//创建http请求头
////	std::string msg;
////	msg = "POST " + _uri.Path() + " HTTP/1.1 \r\n"
////		+ "Host: " + _uri.Host() + "\r\n"
////		+ "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.0; zh-CN; rv:1.9.1.13) Gecko/20100914 Firefox/3.5.13 ( .NET CLR 3.5.30729) \r\n"
////		+ "Connection: close\r\n"
////		+ "Content-type: application/x-www-form-urlencoded\r\n"
////		+ "Content-Length:" + lbuf + " \r\n"
////		+ "\r\n";
////
////	tcp_send(cli, msg.c_str(), msg.length());
////
////	tcp_send(cli, body.c_str(), body.length());
////	
////	while (1) {
////
////		rnum = tcp_recv(cli, buf, 4096);
////		if (rnum>0) response << buf;
////		if (rnum < 1) break;
////	}
////
////	response.Parse();
//
//
///*
//curl_formadd(&formpost,
//&lastptr,
//CURLFORM_COPYNAME, "userfile",
//CURLFORM_FILE, fname,
//CURLFORM_END);
//
//curl_formadd(&formpost,
//&lastptr,
//CURLFORM_COPYNAME, "filename",
//CURLFORM_COPYCONTENTS, fname,
//CURLFORM_END);
//*/
//
//
//	return 0;
//}


//int XHtpGetFile8(const char* href, FILE* outf)
//{
//	return XHtpGetFile9(href,outf);
//}
//
//int XHtpGetFile9(const char* href, FILE* outf)
//{
//	string surl = string(href);
//	HttpResponse response;
//	HttpRequest request;
//	//request.SetMethod("GET");
//	//request.SetURL(surl);
//	request.GetURL(surl,&response);
//
//	//这里要考虑编码方案
//
//	if (response.iContentLength > 0 && response.resultCode==200) {
//		fwrite(response.GetContent(), response.iContentLength, 1, outf);
//		fflush(outf);
////		fclose(outf); //临时
//	}
//	return response.resultCode;
//}



int XHtpGet(string href, string fname)
{
	FILE* outf = fopen(fname.c_str(), "wb");
	if (outf == NULL) return 500;

	string surl = href;
	HttpResponse response;
	HttpRequest request;
	//request.SetMethod("GET");
	//request.SetURL(surl);
	request.GetURL(surl, &response);

	//这里要考虑编码方案

	if (response.iContentLength > 0 && response.resultCode == 200) {
		fwrite(response.GetContent(), response.iContentLength, 1, outf);
		fflush(outf);
		fclose(outf); //临时
	}
	return response.resultCode;
}

