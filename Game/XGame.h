#ifndef XGame_h__
#define XGame_h__



#include "XActor.h"
#include "XPlayer.h"
#include "XScene.h"
#include "opcodes.h"
#include "XListener.h"
#include "XBag.h"
#include "XBagClient.h"
#include "tinyxml2/tinyxml2.h"
#include "XMsgHandler.h"
#include "GxError.h"

#include <stdint.h>
#include <cstdint>
#include <vector>
#include <list>

#define	 X_IDS_AUTH_URL_REQ	"http://mangoschina.blog.163.com/blog/static/27333216120175612634726"
#define  X_IDS_AUTH_URL_PATH "/blog/static/27333216120175612634726"

#define  X_IDS_CLOGIN_FMT "http://%s:%d/login.php?usr=%s&pwd=%s&t=0&app=fqzs&"
#define  X_IDS_CLOGIN_GUEST "http://%s:%d/guest.php"

#define X_MARK_L "&gt;&gt;&gt;&gt;&gt;"
#define X_MARK_R "&lt;&lt;&lt;&lt;&lt;"

using namespace xg;

class GxApplication :public GxObject, public Singleton<GxApplication>
{
public:
	time_t m_tiAuthURL; //最后一次获得的登录
	string m_strAuthURL;

	GxPlayer m_mySelf;
	GxScene  m_myScene;

	string m_acct_id;
	string m_username;	//账号名
	string m_session;  //会话令牌
	string m_password; //密码

	string m_game_host; //游戏服务器主机地址
	int    m_game_port; //游戏服务器端口

	string m_login_host; //登录服务器地址
	int	   m_login_port; //登录服务器端口

	int m_net_pcode;	//客户端协议版本号

	string m_strCfgFilename;

	int m_iLastError;
	string m_write_path_root;
	std::vector<gx_error_t> m_strErrorCnt;

	std::list<gxmsginfo_t> msgHandlers;

	int m_iTimeDelta;

	void LoginGuest();
public:

	static GxScene* Scene();
	static GxPlayer* Self();

	void ConfigDefaultSave(std::string _filename);
public:
	GxBagClient m_bagClient;	//背包客户端
//	tinyxml2::XMLDocument m_cfgDoc;
//	std::string GetValueStringFrom(tinyxml2::XMLElement* _elm, std::string kname);
//	void CfgAttribIntSet(const char* kname,int _val );
//	int  CfgAttribIntGet(const char* kname);
//	void CfgAttribStringSet(const char* kname,const char* _val,size_t _sz);
//  std::string CfgAttribStringGet(const char* kname);


protected:

public:
	GxApplication();
	virtual ~GxApplication();

	GxPlayer& MySelf();
	GxScene&  MyScene();

	void LoadConfigFromXmlFile(const char* fname);

	bool AuthUrlIsExpire();
	void AuthUrlStringGet();
	void AuthUrlSet(std::string str);


	//注册一个游客 也是给玩家一个默认的账号
	void RegisterGuest();

	void OnAfterConnectGame();

	void ResponseSessionRecvAfter();


	void LoginAuto(void*);
	void Login(std::string _url);
	//收到登录返回信息后
	void OnRecvLoginAfter(std::string _cnt);

//	void ConnectGameServer();

	void OnRecvLoginInfo(std::string _host, int _port);

	void SendToServer(const void* buf, size_t sz);

public:
	int OnMessage(char* buf, size_t sz, void* arg);


	void DisplayStringSet(const char* txt);
	const char* DisplayStringGet();

	void LastErrorSet(int n, const char* txt);

	string m_strDisplay;
	string m_strError;
	int	   m_iErrorCode;


	void ErrorPushBack(int n, const char* txt);
	std::string ErrorLastString();
	std::string ErrorString(int idx);

public:
	std::set<GxListener*> m_gxListener;

	void GxListenerAdd(GxListener*);
	void GxListenerDel(GxListener*);

public:
	//通知渲染层的改变
	void RenderDraw(int opc,void* buf,size_t sz,void* p1,void* p2);


	void AgreeWarning(bool _agree, bool _save);
private:
	void SaveUserPwdSidToCfg();
public:
	void LoginUserPassword(string _name, string _password);


	void MsgHandlerAdd(GxMsgHandler* _handler,int _order);
	void MsgHandlerRemove(GxMsgHandler* _handler);
	void MsgHandlerSort();
	void MsgHandlerDespatch(const void* buf,size_t sz,void* arg);


public:

	std::map<uint16_t, std::list<gx_net_msg_handler_t>> mapNetHandler;

	/**
	订阅网络消息
	*/
	void NetMsgHandlerAdd(uint16_t, XNET_MSG_HANDLER, int, void*);
	void NetMsgHandlerRemove(XNET_MSG_HANDLER);

	void NetMsgHandlerDespatch(const void* buf, size_t sz, void* arg);

public:
	void applicationDidEnterBackground();
	void applicationWillEnterForeground();


};

int GameDirectorMsg(char* buf, size_t sz, void* arg);


#endif // XGame_h__
