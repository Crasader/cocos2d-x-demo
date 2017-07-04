/********************************************************

支持游戏中使用 HTTP 协议
	GET
	POST

*********************************************************/

//
//  XHttp.h
//  XNetGame
//
//  Created by im on 12-11-15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
/*
2017-07-04 修改支持连接后发送和接收采用线程
*/

#ifndef XNetGame_XHttp3_h_
#define XNetGame_XHttp3_h_  werqasdfas34345234



#include <string>
#include <list>
#include <map>
#include <unordered_map>

//#include <winsock2.h>
//#include <ws2tcpip.h>
//#pragma comment (lib, "Ws2_32.lib")
//#include "XTcpClient.h"
//#include "MemFile.h"

#include "xnet.h"

using namespace std;


#define XHTTP_HEAD_END  "\r\n\r\n"


#include "ByteBuffer.h"

struct FtpFile   //定义一个结构为了传递给my_fwrite函数.可用curl_easy_setopt的CURLOPT_WRITEDATA选项传递   
{   
	const char *filename;   
	FILE *stream;   
}; 


/*
 返回 0 正确完成
	
 _addr 主机地址
 doc 要请求的文档
 psContent 返回数据的内容
 datSize 数据长度
 rdat 如果为真 就不需要数据了
 */

//int XHtpGet3(const char* url,ByteBuffer* pBBuffer=NULL,bool ssl_on=false,double* _len=NULL);

//int XHtpPost3(const char* url,const char* fname);


extern double xhttp_down_total;		/*当前准备下载文件的总长度*/
extern double xhttp_down_now;		/*目前已经下载了多少*/


class XHtpGetProgressDelegate
{
public:
	virtual void SetDownloadProgress(double rDlNow,double rDlTotal){};
	virtual void SetDownloadTotal(size_t len){}; //将要下载的总长度
	virtual void SetDownloadMessage(const char* text){};
};

/*
void XXHtpGetProgressDelegateSetShared(XHtpGetProgressDelegate* delegate);

//下载文件函数
int XHtpGetFile(const char* url,ByteBuffer* pBBuffer,XHtpGetProgressDelegate* delegate);

int XHtpGetFileSaveTo(const char* url,const char* filename);

int XHtpGetFile2(const char* url,const char* filen,XHtpGetProgressDelegate* delegate,FILE* fstream=NULL);

int XHtpGetFile7(const char* url,const char* filen);

int XHtpGetFile8(const char* href,FILE* outf); //保存到这个文件指针中

int XHtpGetFile9(const char* href,FILE* outf); //保存到这个文件指针中
*/

class HttpResponse
{
public:
	HttpResponse();
	virtual ~HttpResponse();

//	HttpResponse(string& _data);
//	string& Content();

	void Parse();
	void ParseContentForChunked();
	void ParseChunk();

	virtual HttpResponse& operator<< (const string& value);
	virtual HttpResponse& operator<< (const char* value);
	virtual HttpResponse& operator<< (const unsigned char* value);

	void Append(const void* value,size_t sz);

public:
//	unsigned char* m_Content;
//	unsigned char* m_Data;
	ByteBuffer m_bbf; //内容保存
	
//	ByteBuffer m_DataBbf;

	unordered_map<string, string> mapHead;

	int iContentLength;

	const char* GetContent();
	size_t GetContentLength() { return m_Content.size(); };

	ByteBuffer m_Content;

	bool m_b_chunked;

	int resultCode;
	std::string PVer; //协议说明
	std::string ErrorString; //错误提示文本

	std::string GetHeadFeildValueIgnoreCase(std::string kname);

protected:
//	size_t rpos_Content;
//	size_t len_Content;

	size_t rpos_Data;
	size_t len_Data;

	int ihead_len;

protected:
	void CheckDataSize(size_t sz);

};

class HttpRequest
{
public:
	HttpRequest();

public:
	void AddResponseHeader(const char* feild,const char* value);

	void AddResponseHeader(const string& feild, const string& value);

	void SetMethod(const string& method="GET");
	void SetHttpVersion(const string& ver="HTTP/1.1");
//	void SetPath(const char* path = "/");

	//void GetUrlHost()
	void SetSocket(XSOCKET s);
	void SetURL(const string& url);

public:
	string strHead;

	XSOCKET m_socket;

	string m_method;
	string m_http_version;
	string m_url;

	string m_host;
	string m_port;

	string m_uri_original;

public:
	std::map<std::string, std::list<std::string> > m_fields;
	std::map<std::string, std::string> m_files;
	std::string m_boundary;
	std::map<std::string, long> m_content_length;
	std::map<std::string, std::string> m_content_type;

private:
	bool m_bMultipart;

public:
	void DoMultipartPost();
	void SendRequest();

public:
	void Send(const string& dat);
	void SendBuf(const char* lpBuf, size_t sz);
	void SendFile(const string& fname);

public:
	void Perform(HttpResponse* _response);
//	void AddFile(const std::string& name, const std::string& filename, const std::string& type);
	void AddMultilineField(const std::string& name, std::list<std::string>& values);

	void AddField(const std::string& name, const std::string& value);

public:
	void GetURL(string surl,HttpResponse* _response);

	//2017-07-02
	HttpResponse* GetDocAfterConnected();
	std::string GetPath() { return m_url; };

};


//int XHtpGet(string href, string fname);


#endif
