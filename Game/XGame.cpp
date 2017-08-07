
#include "XGame.h"
#include "gayola/ByteBuffer.h"
#include "gayola/CxTcpClient.h"
#include "gayola/CxHttpClient.h"

//#include "tinyxml2/tinyxml2.h"
#include "gayola/rapidxml/rapidxml.hpp"
#include "gayola/rapidxml/rapidxml_iterators.hpp"
#include "gayola/rapidxml/rapidxml_print.hpp"
#include "gayola/rapidxml/rapidxml_utils.hpp"

#include "XProtocol.h"
#include "gayola/CxHttper.h"
#include "gayola/url_encoder.h"

X_IMPL_SINSTANCE(GxApplication)

using namespace xs;
using namespace xg;

void XzSendToClient(const void* buf, size_t sz);
void XzConnectGame(std::string host, int port);
void XzSendToServer(void* buf, size_t sz);
void XzAppMessagePushBack(std::string kname, void* wnd, const char* buf, size_t sz, void* who, bool zip);

void XzDirectorPushBack(const void* buf, size_t sz,void* arg);

#define XAPP_LOGIN_URL "http://mangoschina.blog.163.com/blog/static/27333216120175612634726"


void GxLogDefault(int level, const char* fmt, ...)
{
	char buffer[4096];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buffer, fmt, ap);
	va_end(ap);
	std::cout << buffer << std::endl;
}



GXLOG GxApplication::gLog = GxLogDefault;

void XzSendToClient(const void* buf, size_t sz)
{
	CxTcpClient::shared()->SendData(buf, (int)sz);
}

int GameDirectorMsg(char* buf, size_t sz, void* arg)
{
	return GxApplication::Instance()->OnMessage(buf, sz, arg);
}


GxApplication::GxApplication()
{
	m_login_host = "127.0.0.1";
	m_login_port = 4002;

	NetMsgHandlerAdd(0, XPTO_GAME::NetMsgHandler, -512, this);

	XPTO_GAME::SetFunction("appSendToClient", XzSendToClient);
	XPTO_GAME::Init();
}

GxApplication::~GxApplication()
{

}

void GxApplication::LoginGuest()
{
	m_myScene.ChildRemoveAll();

	Login("http://127.0.0.1:4002/guest.php");
}

GxScene* GxApplication::Scene()
{
	return &GxApplication::Instance()->m_myScene;
}

GxPlayer* GxApplication::Self()
{
	return &GxApplication::Instance()->m_mySelf;
}

void GxApplication::ConfigDefaultSave(std::string _filename)
{
	//tinyxml2::XMLDocument doc;
	//doc.LinkEndChild(doc.NewDeclaration());
	//tinyxml2::XMLElement* root = doc.NewElement("cfg");
	//doc.LinkEndChild(root);
	//doc.SaveFile(_filename.c_str());
}

//std::string GxApplication::GetValueStringFrom(tinyxml2::XMLElement* _elm, std::string kname)
//{
//	std::string res;
//
//	if (_elm && !kname.empty())
//	{
//		tinyxml2::XMLElement* my = _elm->FirstChildElement(kname.c_str());
//		if (my) res = my->GetText();
//	}
//
//	return res;
//}
//
//void GxApplication::CfgAttribIntSet(const char* kname, int _val)
//{
//
//}
//
//int GxApplication::CfgAttribIntGet(const char* kname)
//{
//	return 0;
//}
//
//void GxApplication::CfgAttribStringSet(const char* kname, const char* _val, size_t _sz)
//{
//
//}
//
//std::string GxApplication::CfgAttribStringGet(const char* kname)
//{
//	std::string str;
//	return str;
//}


GxPlayer& GxApplication::MySelf()
{
	return m_mySelf;
}

GxScene& GxApplication::MyScene()
{
	return m_myScene;
}

void GxApplication::LoadConfigFromXmlFile(const char* fname)
{
	assert(fname);
	//m_strCfgFilename = fname;
	//m_cfgDoc.LoadFile(fname);
	//if (m_cfgDoc.Error()) {
	//	LastErrorSet(m_cfgDoc.ErrorID(), m_cfgDoc.GetErrorStr1());
	//	return;
	//}

	//tinyxml2::XMLElement* root = m_cfgDoc.RootElement();
	//if (root)
	//{
	//	if (root->Attribute("username")) m_username = root->Attribute("username");
	//	if (root->Attribute("password")) m_password = root->Attribute("password");
	//}

}

bool GxApplication::AuthUrlIsExpire()
{
	return true;
}

void GxApplication::AuthUrlStringGet()
{

}

void GxApplication::RegisterGuest()
{
	Login("http://127.0.0.1:4002/guest.php");
}

void GxApplication::OnAfterConnectGame()
{
	//����Ϸ���������ͻỰ������֤
	//opcode
	//Э���
	//�˺�+�Ự
	//��¼������IP+port
	//������Ϣ����
	ByteBuffer bbf;
	bbf << (uint16_t)XCMSG_SESSION;
	bbf << m_acct_id;
	bbf << m_session;
	bbf << m_login_host;
	bbf << m_login_port;
	bbf << m_net_pcode;
	
	CxTcpClient::shared()->SendData(bbf.contents(), bbf.size());

}

void GxApplication::ResponseSessionRecvAfter()
{
	//�����ɫ�б�
	//XPTO_GAME::c_char_enum();

}



void GxApplication::LoginAuto(void* arg)
{
	//��ȡ��¼������IP+Port
	CxHttpClient::Instance()->ThGet(XAPP_LOGIN_URL, XzAppMessagePushBack, arg);
}

void GxApplication::Login(std::string _url)
{
	//�����������HTTP����
	CxHttpClient::Instance()->SetCookie("game.login.guest");
	CxHttpClient::Instance()->ThGet(_url, XzAppMessagePushBack, this);
}


#define GetValueStringFrom(b,a,c) \
	{rapidxml::xml_node<> * my = b->first_node(a); \
		if (my) c = my->contents(); }

/*

<?xml version='1.0' encoding='utf-8' ?>
<guest>
<error>0</error>
<user>557479446</user>
<password>123456</password>
<session>wljpsatdapfkmwwxbqyuapweqyxttqgo</session>
<game>127.0.0.1:4002</game>
</guest>

*/

void GxApplication::OnRecvLoginAfter(std::string _cnt)
{
	//����XML���
	using namespace rapidxml;
	xml_document<> doc;
	try {
		doc.parse<0>((char*)_cnt.c_str());
	}
	catch (rapidxml::parse_error e) {
		std::string errStr = e.what();
		//TODO
		return;
	}

	//
	xml_node<> *root = doc.first_node();
	if (root == NULL) return;

	if (0 == strcmp(root->name(), "guest"))
	{
		xml_node<> * elm = root->first_node("user");
		if (elm && elm->contents()) {
			m_username = elm->contents();
		}

		//����
		elm = root->first_node("password");
		if (elm && elm->contents()) {
			m_password = elm->contents();
		}

		//�Ự����
		elm = root->first_node("session");
		if (elm && elm->contents()) {
			m_session = elm->contents();
		}

		//acctid
		elm = root->first_node("acctid");
		if (elm && elm->contents()) {
			m_acct_id = elm->contents();
		}

		SaveUserPwdSidToCfg();

		m_iLastError = 0;

	}

	//
	if (0 == strcmp(root->name(), "login_client_error"))
	{
		//������
		xml_node<> * elm = root->first_node("error");
		if (elm && elm->contents()) {
			m_iLastError = atoi(elm->contents());
		}

		//����Ϣ�㷢��������Ϣ
	}

	if (0 == strcmp(root->name(), "login_client_passed"))
	{
		//��֤ͨ���� ����Ҫ�����˺źͻỰ����
		GetValueStringFrom(root, "acctid", m_acct_id);
		GetValueStringFrom(root, "session", m_session);
		m_iLastError = 0;
	}

	//��Ϸ�����������Ϣ ip+port
	xml_node<>* elm = root->first_node("game");
	if (elm) {
		string str = elm->contents();
		int pos = str.find(":");
		m_game_host = str.substr(0, pos);
		str = str.substr(pos + 1);
		m_game_port = std::atoi(str.c_str());

		XzConnectGame(m_game_host, m_game_port);
	}


	//����������
	m_mySelf.m_acct_uuid = m_acct_id;


	//����Ϣ�������͵�¼�ɹ�����Ϣ
	ByteBuffer bbf;
	bbf << (uint16_t)XXMSG_LOGIN;
	bbf << m_iLastError;
	MsgHandlerDespatch(bbf.contents(), bbf.size(), 0);

}
/*

<?xml version='1.0' encoding='utf-8' ?>
<guest>
<error>0</error>
<user>557479446</user>
<password>123456</password>
<session>wljpsatdapfkmwwxbqyuapweqyxttqgo</session>
<game>127.0.0.1:4002</game>
</guest>

*/

#if(0)
void GxApplication::OnRecvLoginAfter(std::string _cnt)
{
	//����XML���
	tinyxml2::XMLDocument doc;
	doc.Parse(_cnt.c_str());
	if (doc.Error())
	{
		//FIXME
		return;
	}

	//
	tinyxml2::XMLElement* root = doc.RootElement();
	if (0 == strcmp(root->Name(), "guest"))
	{
		tinyxml2::XMLElement* elm = root->FirstChildElement("user");
		if (elm && elm->GetText()) {
			m_username = elm->GetText();
		}

		//����
		elm = root->FirstChildElement("password");
		if (elm && elm->GetText()) {
			m_password = elm->GetText();
		}

		//�Ự����
		elm = root->FirstChildElement("session");
		if (elm && elm->GetText()) {
			m_session = elm->GetText();
		}

		SaveUserPwdSidToCfg();

		m_iLastError = 0;

	}

	//
	if (0 == strcmp(root->Name(), "login_client_error"))
	{
		//������
		tinyxml2::XMLElement* elm = root->FirstChildElement("error");
		if (elm && elm->GetText()) {
			 m_iLastError= atoi(elm->GetText());
		}

		//����Ϣ�㷢��������Ϣ
	}

	if (0 == strcmp(root->Name(), "login_client_passed"))
	{
		//��֤ͨ���� ����Ҫ�����˺źͻỰ����
		m_acct_id = GetValueStringFrom(root, "acctid");
		m_session = GetValueStringFrom(root, "session");
		m_iLastError = 0;
	}

	//��Ϸ�����������Ϣ ip+port
	tinyxml2::XMLElement* elm = root->FirstChildElement("game");
	if (elm) {
		string str = elm->GetText();
		int pos = str.find(":");
		m_game_host = str.substr(0, pos);
		str = str.substr(pos + 1);
		m_game_port = std::atoi(str.c_str());

		XzConnectGame(m_game_host, m_game_port);
	}


	//����������
	m_mySelf.m_acct_uuid = m_acct_id;



	//����Ϣ�������͵�¼�ɹ�����Ϣ
	ByteBuffer bbf;
	bbf << (uint16_t)XXMSG_LOGIN;
	bbf << m_iLastError;
	MsgHandlerDespatch(bbf.contents(),bbf.size(),0);

}
#endif



void GxApplication::OnRecvLoginInfo(std::string _host, int _port)
{
	//��װ��¼����
	//��ȡ��¼�˺ź�����
	if (m_username.empty() || m_password.empty())
	{

	}
	else {

	}

	Login("http://127.0.0.1:4002/guest.php");

}

void GxApplication::SendToServer(const void* buf, size_t sz)
{
	//XzSendToServer(buf, sz);
	CxTcpClient::shared()->SendData(buf, sz);
}

int GxApplication::OnMessage(char* buf, size_t sz, void* arg)
{

	ByteReader brr(buf, sz);
	uint16_t msgId = brr.read<uint16_t>();
	if (msgId == XSMSG_AUTH_URL)
	{
		brr >> m_strDisplay;
		DisplayStringSet(m_strDisplay.c_str());
		//FIXME �����յ�������
		OnRecvLoginInfo("127.0.0.1",4002);
		return 1;
	}

	if (msgId == XSMSG_LOGIN)
	{
		std::string cnt;
		brr >> cnt;
		OnRecvLoginAfter(cnt);
		return 1;
	}

	if (msgId == XXMSG_TCP_EVENT)
	{
		int state_name;
		int state_val;
		int arg;
		brr >> state_name;
		brr >> state_val;
		brr >> arg;

		if (state_name == XTCS_READY && arg == 1)
		{
			//TODO ���ͻỰ��¼����
			OnAfterConnectGame();
		}
		return 0;
	}


	//����������Ϣ�ĺ�����������

	NetMsgHandlerDespatch(buf, sz, arg);

	return 0;
}

void GxApplication::DisplayStringSet(const char* txt)
{
	if (txt == NULL) return;
	m_strDisplay = string(txt);
	for (auto it : m_gxListener) {
		it->OnChangeDisplyString(m_strDisplay.c_str());
	}
}

const char* GxApplication::DisplayStringGet()
{
	return m_strDisplay.c_str();
}

void GxApplication::LastErrorSet(int n, const char* txt)
{
	m_iErrorCode = n;
	if(txt) m_strError = string(txt);
	for (auto it : m_gxListener) {
		it->OnError(n, m_strError.c_str());
	}

}

void GxApplication::ErrorPushBack(int n, const char* txt)
{
	m_strErrorCnt.push_back(gx_error_t(n,txt));
}

std::string GxApplication::ErrorLastString()
{
	if (m_strErrorCnt.size() == 0) return "";
	return std::string(m_strErrorCnt.front().text);
}

std::string GxApplication::ErrorString(int idx)
{
	if (m_strErrorCnt.size() <= idx || idx < 0) return "";
	return std::string(m_strErrorCnt[idx].text);
}

void GxApplication::GxListenerAdd(GxListener* arg)
{
	m_gxListener.insert(arg);
}

void GxApplication::GxListenerDel(GxListener* arg)
{
	m_gxListener.erase(arg);
}

void GxApplication::RenderDraw(int opc, void* buf, size_t sz, void* p1, void* p2)
{

}

void GxApplication::AgreeWarning(bool _agree, bool _save)
{
	//���¾����趨
	//if (m_cfgDoc.Error()) {
	//	LastErrorSet(-1, "�����ļ�����");
	//	return;
	//}

	//tinyxml2::XMLElement* root= m_cfgDoc.RootElement();

	//root->SetAttribute("warning",_agree);

	//m_cfgDoc.SaveFile(m_strCfgFilename.c_str());
}

void GxApplication::SaveUserPwdSidToCfg()
{
//	tinyxml2::XMLElement* root= m_cfgDoc.RootElement();
//	if (root == NULL) {
//		m_cfgDoc.LinkEndChild(m_cfgDoc.NewDeclaration());
//		root = m_cfgDoc.NewElement("cfg");
//		m_cfgDoc.LinkEndChild(root);
//	}
//	
///*
//	tinyxml2::XMLElement* elm=root->FirstChildElement("username");
//	if (elm==NULL){
//		elm = m_cfgDoc.NewElement("username");
//		root->LinkEndChild(elm);
//	}
//
//	tinyxml2::XMLText* txt = m_cfgDoc.NewText(m_username.c_str());
//	elm->LinkEndChild(txt);
//*/	
//	//
//	root->SetAttribute("username", m_username.c_str());
//
//	//����
//	root->SetAttribute("password", m_password.c_str());
//
//
//
//
//	m_cfgDoc.SaveFile(m_strCfgFilename.c_str());

}

void GxApplication::LoginUserPassword(string _name, string _password)
{

	m_username = _name;
	m_password = _password;

	//http://192.168.50.190:6002/login.php?usr=13123&pwd=34234&app=2342&pcode=123&t=0&
	char buf[1024];
	UrlEncoder::Encode(_name);
	UrlEncoder::Encode(_password);

	sprintf(buf, "http://%s:%d/login.php?usr=%s&pwd=%s&app=2342&pcode=123&t=0&", m_login_host.c_str(), m_login_port, _name.c_str(), _password.c_str());
	
	CxHttpClient::Instance()->SetCookie("game.login.user");
	CxHttpClient::Instance()->ThGet(buf, XzAppMessagePushBack, this);

}

void GxApplication::MsgHandlerAdd(GxMsgHandler* _handler, int _order)
{
	MsgHandlerRemove(_handler);
	gxmsginfo_t _h = { _handler,_order };
	msgHandlers.push_back(_h);
	MsgHandlerSort();
}

void GxApplication::MsgHandlerRemove(GxMsgHandler* _handler)
{
	for (auto it = msgHandlers.begin(); it != msgHandlers.end();)
	{
		if (it->m_handler == _handler) {
			it = msgHandlers.erase(it);
		}
		else it++;
	}
}

void GxApplication::MsgHandlerSort()
{
	msgHandlers.sort([](const gxmsginfo_t& a, const gxmsginfo_t& b) {return a.m_order < b.m_order; });
}

void GxApplication::MsgHandlerDespatch(const void* buf, size_t sz, void* arg)
{
	for (auto it = msgHandlers.begin(); it != msgHandlers.end();it++)
	{
		if(0!=it->m_handler->OnGxMessage((const char*)buf,sz,arg)) break;
	}
}

void GxApplication::NetMsgHandlerAdd(uint16_t msgId, XNET_MSG_HANDLER proc, int order,void* userdata)
{
	NetMsgHandlerRemove(proc);

	gx_net_msg_handler_t msgHandler = {proc,userdata,order};

	mapNetHandler[msgId].push_back(msgHandler);
	std::list<gx_net_msg_handler_t>& cnt = mapNetHandler[msgId];
	cnt.sort([](const gx_net_msg_handler_t& a,const gx_net_msg_handler_t& b) {
		return a.order < b.order;
	});
}

void GxApplication::NetMsgHandlerRemove(XNET_MSG_HANDLER proc)
{
	for (auto it = mapNetHandler.begin(); it != mapNetHandler.end();it++)
	{
		std::list<gx_net_msg_handler_t>& cnt = (it->second);
		for (auto iit = cnt.begin(); iit != cnt.end();) {
			if (iit->handler == proc)
			{
				iit = cnt.erase(iit);
			}
			else iit++;
		}
	}
}

void GxApplication::NetMsgHandlerDespatch(const void * buf, size_t sz, void * arg)
{
	uint16_t* msgId = (uint16_t*)buf;
	std::list<gx_net_msg_handler_t>& cnt = mapNetHandler[*msgId];
	for (auto it : cnt) {
		(it.handler)((const char*)buf, sz, arg, it.userdata);
	}

	//���͸������Ϣ����
	std::list<gx_net_msg_handler_t>& cnt0 = mapNetHandler[0];
	for (auto it : cnt0) {
		(it.handler)((const char*)buf, sz, arg, it.userdata);
	}

}

void GxApplication::applicationDidEnterBackground()
{
	//�Ͽ����粢��
}

void GxApplication::applicationWillEnterForeground()
{
	//�ж��뿪��ʱ�䳤�� ���̫���ˣ����������¿�ʼ ������Ƕ��ݵľ�ֱ��������Ϸ���ط�����
	//����ֱ��ʹ�þɵĽ�ɫ

}


