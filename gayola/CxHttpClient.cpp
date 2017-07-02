
#include "CxHttpClient.h"
#include "CxThreadEx.h"
#include "CxTcpClient.h"

void WkGetDo(XSOCKET* sk, DIDNETMESSAGE proc, void* n)
{
	char buffer[4096];
	std::string result;
	int r = 0;
	size_t content_length=0;

	do {

		r = ::recv(*sk, buffer, 4096, 0);
		if (r > 0)
		{

			result.append(buffer, r);

			//开始分析收到后的内容

			//查找是否有内容长度的标记

			//查找内容开始的标记 \r\n\r\n

		}

	} while (r > 0);

	proc(n, (char*)result.c_str(), result.length(), 0);

}


void CxHttpClient::ThGet(std::string uri, DIDNETMESSAGE proc, void* n)
{

	XSOCKET sk=Zx::SocketConnect();
	std::thread* _work = new std::thread();
	CxThreadPool::Instance()->Add(_work);
}

