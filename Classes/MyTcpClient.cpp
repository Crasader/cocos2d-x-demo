
#include "MyTcpClient.h"

#include "cocos2d.h"

using namespace cocos2d;

CCTcpClient* CCTcpClient::_shared = NULL;

void CCTcpClient::Step()
{
	m_tcpClient.Step();

	//


}

CCTcpClient* CCTcpClient::shared()
{
	if (_shared == NULL) {
		_shared = new CCTcpClient();
	}
	return _shared;
}

CCTcpClient::CCTcpClient()
{
	xs::CxTcpClient::Startup();

	
	m_tcpClient.AddListener(this);
}

CCTcpClient::~CCTcpClient()
{
	
}

int CCTcpClient::Connect(std::string host, int port)
{
	return m_tcpClient.Connect(host, port);
}




int CCTcpClient::SendToServer(const char* buf, int sz)
{
	m_tcpClient.SendData(buf, sz);
	return 0;
}

void CCTcpClient::OnTcpClientDataRecv(CxTcpClient* sender, const char* _txt, int sz)
{
	//if (sender == &m_tcpClient)
	//{

	//	{
	//		Node* n = Director::getInstance()->getRunningScene()->getChildByName("main");
	//		if (n) {
	//			n->OnMessageRecv((char*)_txt, sz, 0);
	//			GxMsgQueue::shared()->LoopWork([](const char * buf, size_t sz, const void* who, std::string& name, void * arg)
	//			{
	//				Node* n = (Node*)arg;
	//				n->OnMsg(buf, sz, who, name, arg);
	//			}, n);
	//		}
	//	}
	//}
}


void CCTcpClient::OnTcpClientStateChange(CxTcpClient* sender, int state_name, int state_val, int arg)
{
	if (sender == &m_tcpClient)
	{
		//if(state_name==)
		//		GxMsgQueue::shared()->PushBack(0, 0, 0, "OnTcpClientStateChange");
	}
}



void DIRECTOR_TICK_TCPCLIENT(float,void* arg)
{
	CCTcpClient* tcp = (CCTcpClient*)arg;
	tcp->Step();
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void XzOnNetMessage(void* wnd, char* buf, size_t sz, void* arg)
{
	Node* n = (Node*)wnd;
	if (n) n->OnNetMessage(buf, sz, arg);
}
