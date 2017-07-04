#ifndef CCTcpClient_h__
#define CCTcpClient_h__

#include "gayola/CxTcpClient.h"

using namespace xs;

class CCTcpClient : public CxTcpClientListener
{
private:
	static CCTcpClient* _shared;
protected:

	


public:
	CxTcpClient* m_tcpClient;

	virtual void Step();

public:
	static CCTcpClient* shared();
	CCTcpClient();
	virtual ~CCTcpClient();





public:
	int Connect(std::string host, int port);
	int SendToServer(const char* buf, int sz);

	virtual void OnTcpClientDataRecv(CxTcpClient* sender, const char* _txt, int sz);
	virtual void OnTcpClientStateChange(CxTcpClient* sender, int state_name, int state_val, int arg);
};


void DIRECTOR_TICK_TCPCLIENT(float,void*);

void XzOnNetMessage(void* wnd,char* buf, size_t sz, void* arg);
void XzOnAppMessage(void* wnd,char* buf, size_t sz, void* who);


void XzAppMessagePushBack(std::string kname,void* wnd, const char* buf, size_t sz, void* who,bool zip);

void XzSendToServer(void* buf,size_t sz);

#endif // CCTcpClient_h__
