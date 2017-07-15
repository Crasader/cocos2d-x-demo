#ifndef CxHttpClient_h__
#define CxHttpClient_h__

#include <string>
#include <stdlib.h>
#include <stdio.h>



/*
Android 必须在主线程中执行
	socket connect 
	fopen 
*/


typedef void(*DIDNETMESSAGE)(void* wnd, char* buf, size_t sz, void* arg);

class CxHttpClient
{
private:
	static CxHttpClient* _shared;

public:
	static CxHttpClient* Instance();

	std::string m_cookie;

public:
//	static void ThreadGetDocumentToFile(std::string uri, FILE* pf);
//	static void ThreadGetDocumentToString(std::string uri, std::string& result);

	void ThGet(std::string _uri, void* proc,void* n);
	void SetCookie(std::string _cookie);

	void Disconnect();
};



#endif // CxHttpClient_h__
