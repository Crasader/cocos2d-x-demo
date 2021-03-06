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

typedef void(*GXLOG)(int, const char* fmt, ...);

#define  XLOG_TRACE(fmt,...)   GxApplication::gLog(0,fmt " %s %d \n" ,##__VA_ARGS__,__FILE__,__LINE__)
#define  XLOG_DEBUG(fmt,...)    GxApplication::gLog(1,fmt " %s %d \n" ,##__VA_ARGS__,__FILE__,__LINE__)
#define  XLOG_INFO(fmt,...)     GxApplication::gLog(2,fmt " %s %d \n" ,##__VA_ARGS__,__FILE__,__LINE__)
#define  XLOG_WARN(fmt,...)    GxApplication::gLog(3,fmt " %s %d \n" ,##__VA_ARGS__,__FILE__,__LINE__)
#define  XLOG_ALARM(fmt,...)  GxApplication::gLog(4,fmt " %s %d \n" ,##__VA_ARGS__,__FILE__,__LINE__)
#define  XLOG_ERROR(fmt,...)    GxApplication::gLog(5,fmt " %s %d \n" ,##__VA_ARGS__,__FILE__,__LINE__)
#define  XLOG_FATAL(fmt,...)    GxApplication::gLog(6,fmt " %s %d \n" ,##__VA_ARGS__,__FILE__,__LINE__)


#define  X_IDOK  0x00000000L
#define  X_IDCANCEL 0x80000000L
#define  X_IDTRY 0x40000000L
#define  X_IDYES 0x00000000L
#define  X_IDNO  0x40000000L

#ifndef DWORD	
typedef unsigned long       DWORD;
#endif

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

	static GXLOG gLog;
	
public:
	GxBagClient m_bagClient;	//背包客户端



protected:

public:
	GxApplication();
	virtual ~GxApplication();

	GxPlayer& MySelf();
	GxScene&  MyScene();

	void ConfigLoadFromFile(std::string _filename);
	void ConfigSaveToFile(std::string _filename);

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
