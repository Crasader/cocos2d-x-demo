
#include "XGame.h"
#include "gayola/ByteBuffer.h"
#include "gayola/CxTcpClient.h"
#include "gayola/CxHttpClient.h"

#include "tinyxml2/tinyxml2.h"

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


void XzSendToClient(const void* buf, size_t sz)
{
	CxTcpClient::shared()->SendData(buf, (int)sz);
}

int GameDirectorMsg(char* buf, size_t sz, void* arg)
{
	return GxApplication::Instance()->OnMessage(buf, sz, arg);
}


void GxApplication::LoginGuest()
{
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
	tinyxml2::XMLDocument doc;
	doc.LinkEndChild(doc.NewDeclaration());
	tinyxml2::XMLElement* root = doc.NewElement("cfg");
	doc.LinkEndChild(root);
	doc.SaveFile(_filename.c_str());
}

std::string GxApplication::GetValueStringFrom(tinyxml2::XMLElement* _elm, std::string kname)
{
	std::string res;

	if (_elm && !kname.empty())
	{
		tinyxml2::XMLElement* my = _elm->FirstChildElement(kname.c_str());
		if (my) res = my->GetText();
	}

	return res;
}

void GxApplication::CfgAttribIntSet(const char* kname, int _val)
{

}

int GxApplication::CfgAttribIntGet(const char* kname)
{
	return 0;
}

void GxApplication::CfgAttribStringSet(const char* kname, const char* _val, size_t _sz)
{

}

std::string GxApplication::CfgAttribStringGet(const char* kname)
{
	std::string str;
	return str;
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
	m_strCfgFilename = fname;
	m_cfgDoc.LoadFile(fname);
	if (m_cfgDoc.Error()) {
		LastErrorSet(m_cfgDoc.ErrorID(), m_cfgDoc.GetErrorStr1());
		return;
	}

	tinyxml2::XMLElement* root = m_cfgDoc.RootElement();
	if (root)
	{
		if (root->Attribute("username")) m_username = root->Attribute("username");
		if (root->Attribute("password")) m_password = root->Attribute("password");
	}

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
	//向游戏服务器发送会话令牌验证
	//opcode
	//协议号
	//账号+会话
	//登录服务器IP+port
	//其他信息附加
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
	//请求角色列表
	//XPTO_GAME::c_char_enum();

}



void GxApplication::LoginAuto(void* arg)
{
	//获取登录服务器IP+Port
	CxHttpClient::Instance()->ThGet(XAPP_LOGIN_URL, XzAppMessagePushBack, arg);
}

void GxApplication::Login(std::string _url)
{
	//向服务器发送HTTP请求
	CxHttpClient::Instance()->SetCookie("game.login.guest");
	CxHttpClient::Instance()->ThGet(_url, XzAppMessagePushBack, this);
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

void GxApplication::OnRecvLoginAfter(std::string _cnt)
{
	//分析XML结果
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

		//密码
		elm = root->FirstChildElement("password");
		if (elm && elm->GetText()) {
			m_password = elm->GetText();
		}

		//会话令牌
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
		//错误编号
		tinyxml2::XMLElement* elm = root->FirstChildElement("error");
		if (elm && elm->GetText()) {
			 m_iLastError= atoi(elm->GetText());
		}

		//向消息层发布错误消息
	}

	if (0 == strcmp(root->Name(), "login_client_passed"))
	{
		//验证通过了 这里要设置账号和会话令牌
		m_acct_id = GetValueStringFrom(root, "acctid");
		m_session = GetValueStringFrom(root, "session");
		m_iLastError = 0;
	}

	//游戏接入服务器信息 ip+port
	tinyxml2::XMLElement* elm = root->FirstChildElement("game");
	if (elm) {
		string str = elm->GetText();
		int pos = str.find(":");
		m_game_host = str.substr(0, pos);
		str = str.substr(pos + 1);
		m_game_port = std::atoi(str.c_str());

		XzConnectGame(m_game_host, m_game_port);
	}


	//分析错误结果



	//向消息监听发送登录成功的消息
	ByteBuffer bbf;
	bbf << (uint16_t)XXMSG_LOGIN;
	bbf << m_iLastError;
	MsgHandlerDespatch(bbf.contents(),bbf.size(),0);

}

void GxApplication::OnRecvLoginInfo(std::string _host, int _port)
{
	//组装登录请求
	//读取登录账号和密码
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
		//FIXME 分析收到的内容
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
			//TODO 发送会话登录请求
			OnAfterConnectGame();
		}
		return 0;
	}


	//这里向订阅消息的函数发布内容

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
	//更新警告设定
	if (m_cfgDoc.Error()) {
		LastErrorSet(-1, "配置文件错误");
		return;
	}

	tinyxml2::XMLElement* root= m_cfgDoc.RootElement();

	root->SetAttribute("warning",_agree);

	m_cfgDoc.SaveFile(m_strCfgFilename.c_str());
}

void GxApplication::SaveUserPwdSidToCfg()
{
	tinyxml2::XMLElement* root= m_cfgDoc.RootElement();
	if (root == NULL) {
		m_cfgDoc.LinkEndChild(m_cfgDoc.NewDeclaration());
		root = m_cfgDoc.NewElement("cfg");
		m_cfgDoc.LinkEndChild(root);
	}
	
/*
	tinyxml2::XMLElement* elm=root->FirstChildElement("username");
	if (elm==NULL){
		elm = m_cfgDoc.NewElement("username");
		root->LinkEndChild(elm);
	}

	tinyxml2::XMLText* txt = m_cfgDoc.NewText(m_username.c_str());
	elm->LinkEndChild(txt);
*/	
	//
	root->SetAttribute("username", m_username.c_str());

	//密码
	root->SetAttribute("password", m_password.c_str());




	m_cfgDoc.SaveFile(m_strCfgFilename.c_str());

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

	//发送给零的消息订阅
	std::list<gx_net_msg_handler_t>& cnt0 = mapNetHandler[0];
	for (auto it : cnt0) {
		(it.handler)((const char*)buf, sz, arg, it.userdata);
	}

}

void GxApplication::applicationDidEnterBackground()
{
	//断开网络并且
}

void GxApplication::applicationWillEnterForeground()
{
	//判断离开的时间长短 如果太长了，就整个重新开始 如果仅是短暂的就直接连接游戏网关服务器
	//并且直接使用旧的角色

}

