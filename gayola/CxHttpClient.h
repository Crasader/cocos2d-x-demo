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

public:
	static void ThreadGetDocumentToFile(std::string uri, FILE* pf);
	static void ThreadGetDocumentToString(std::string uri, std::string& result);

	static void ThGet(std::string uri, DIDNETMESSAGE proc,void* n);

};



#endif // CxHttpClient_h__
