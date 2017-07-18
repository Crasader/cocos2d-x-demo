
#include "XGame.h"
#include "gayola/ByteBuffer.h"
#include "gayola/CxTcpClient.h"
#include "gayola/CxHttpClient.h"

#include "tinyxml2/tinyxml2.h"

X_IMPL_SINSTANCE(GxApplication)

using namespace xs;

void XzConnectGame(std::string host, int port);
void XzSendToServer(void* buf, size_t sz);
void XzAppMessagePushBack(std::string kname, void* wnd, const char* buf, size_t sz, void* who, bool zip);

void XzDirectorPushBack(const void* buf, size_t sz,void* arg);

#define XAPP_LOGIN_URL "http://mangoschina.blog.163.com/blog/static/27333216120175612634726"

int GameDirectorMsg(char* buf, size_t sz, void* arg)
{
	return GxApplication::Instance()->OnMessage(buf, sz, arg);
}


void GxApplication::ConfigDefaultSave(std::string _filename)
{
	tinyxml2::XMLDocument doc;
	doc.LinkEndChild(doc.NewDeclaration());
	tinyxml2::XMLElement* root = doc.NewElement("cfg");
	doc.LinkEndChild(root);
	doc.SaveFile(_filename.c_str());
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

}

void GxApplication::OnAfterConnectGame()
{
	//向游戏服务器发送会话令牌验证
	//opcode
	//协议号
	//账号+会话
	//登录服务器IP+port
	
}

void GxApplication::ResponseSessionRecvAfter()
{
	//请求角色列表

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



	}

	//游戏接入服务器信息 ip+port
	tinyxml2::XMLElement* elm = root->FirstChildElement("game");
	if (elm) {
		string str = elm->GetText();
		int pos = str.find(":");
		m_game_host = str.substr(0, pos);
		str = str.substr(pos + 1);
		m_game_port = std::atoi(str.c_str());
	}

	//ByteBuffer bbf;
	//bbf << (uint16_t)XCCOP_TCP_CONNECT;
	//XzDirectorPushBack(bbf.contents(), bbf.size(), this);
	XzConnectGame(m_game_host, m_game_port);

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

