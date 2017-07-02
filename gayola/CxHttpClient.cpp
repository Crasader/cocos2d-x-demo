
#include "CxHttpClient.h"
#include "CxThreadEx.h"
#include "CxTcpClient.h"

#include "http_parser.h"
#include "CxUri.h"
#include "xnet.h"


/*

HTTP/1.1 403 Forbidden
Server: nginx
Date: Sun, 02 Jul 2017 05:56:41 GMT
Content-Type: text/html
Content-Length: 162
Connection: close
Vary: Accept-Encoding

*/


struct htp_work_item
{
	XSOCKET sk;
	std::string request;
	DIDNETMESSAGE proc;
	void* n;
};

typedef struct htp_work_item  htp_work_item_t;

void WkGetDo(void* arg)
{
	struct htp_work_item* _item = (htp_work_item_t*)arg;
	char buffer[4096];
	std::string result;
	int r = 0;
	int cont_start = 0;
	size_t content_length=0;
	size_t content_length_r = 0;

	bool bcon_start=false;

	//��������
//	sprintf(buffer, "GET %s HTTP/1.1\r\n\r\n", _item->doc.c_str());
//	xnet_send(_item->sk, buffer, strlen(buffer));
	xnet_send(_item->sk, _item->request.c_str(), _item->request.length());

	do {
		int bsz = content_length- content_length_r;
		if (bsz > 4096 || bsz<1) bsz = 4096;

		r = ::recv(_item->sk, buffer, bsz, 0);
		if (r > 0)
		{

			result.append(buffer, r);

			//��ʼ�����յ��������

			if (!bcon_start) {
				//�����Ƿ������ݳ��ȵı��
				int p = result.find("Content-Length");
				if (p != std::string::npos) {
					int a = strlen("Content-Length") + p+1;
					int b = result.find("\r",a);
					std::string cnlen = result.substr(a, b -a);
					content_length = std::strtoull(cnlen.c_str(), NULL, 10);
				}

				//�������ݿ�ʼ�ı�� \r\n\r\n
				p= result.find("\r\n\r\n");
				if (p != std::string::npos) {
					cont_start = p + 4;
					bcon_start = true;
					content_length_r = result.length() - cont_start;

					//�Ѿ��յ�ȫ�� ���ܶ�������
					if(content_length_r>= content_length) break;

				}

			}
			else {
				content_length_r += r;
			}

			
			

		}

	} while (r > 0);

	_item->proc(_item->n, (char*)result.c_str()+ cont_start, result.length()- cont_start, 0);

	delete _item;

}


void CxHttpClient::ThGet(std::string _uri, DIDNETMESSAGE proc, void* n)
{
	Zx::Uri my_uri=Zx::Uri::parse(_uri);
	int i = 0;
	struct in_addr addr;

	//struct http_parser_url _url;

	htp_work_item_t* wit = new htp_work_item_t();
	wit->proc = proc;
	wit->n = n;

	struct hostent * remoteHost = xnet_gethostbyname(my_uri.getHost().c_str());
	if (remoteHost == NULL) return;

	if (remoteHost->h_addrtype == AF_INET)
	{
		i = 0;
		while (remoteHost->h_addr_list[i] != 0) {
			addr.s_addr = *(u_long *)remoteHost->h_addr_list[i++];
			printf("\tIP Address #%d: %s\n", i, inet_ntoa(addr));
			break;
		}
	}

	const char* ipAddr = inet_ntoa(addr);
	int port = my_uri.getPort();
	if (port == 0) port = 80;

	wit->sk = xnet_connect(ipAddr, port);// Zx::SocketConnectEx(my_uri.getHost(), 80);

	//wit->doc = my_uri.getPath();
	//char buffer[4096];
	//snprintf(buffer, 4096, "GET %s HTTP/1.1\nHost:%s\nAccept: text/html\n\r\n\r\n",
	//	my_uri.getPath().c_str(),
	//	my_uri.getHost().c_str()
	//	);


	
	wit->request = "GET " + my_uri.getPath() + " HTTP/1.0 \n"
		+ "Host: " + my_uri.getHost() + "\n"
		+ "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.0; zh-CN; rv:1.9.1.13) Gecko/20100914 Firefox/3.5.13 ( .NET CLR 3.5.30729) \r\n\r\n";



//	wit->request = buffer;

	std::thread* _work = new std::thread(WkGetDo,wit);
	CxThreadPool::Instance()->Add(_work);
}

