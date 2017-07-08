#ifndef XGame_h__
#define XGame_h__


#include "XActor.h"
#include "XPlayer.h"
#include "XScene.h"
#include "opcodes.h"
#include "XListener.h"
#include "XBag.h"
#include "tinyxml2/tinyxml2.h"

#define	 X_IDS_AUTH_URL_REQ	"http://mangoschina.blog.163.com/blog/static/27333216120175612634726"
#define  X_IDS_AUTH_URL_PATH "/blog/static/27333216120175612634726"

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

	string m_session;  //会话令牌
	string m_password; //密码

	string m_game_host; //游戏服务器主机地址
	int    m_game_port; //游戏服务器端口

	string m_strCfgFilename;

public:
	GxBagClient m_bagClient;	//背包客户端
	tinyxml2::XMLDocument m_cfgDoc;


	void CfgAttribIntSet(const char* kname,int _val );
	int  CfgAttribIntGet(const char* kname);

	void CfgAttribStringSet(const char* kname,const char* _val,size_t _sz);
	std::string CfgAttribStringGet(const char* kname);


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
	void Login();
//	void ConnectGameServer();

	void SendToServer(const void* buf, size_t sz);

public:
	int OnMessage(char* buf, size_t sz, void* arg);


	void DisplayStringSet(const char* txt);
	const char* DisplayStringGet();

	void LastErrorSet(int n, const char* txt);

	string m_strDisplay;
	string m_strError;
	int	   m_iErrorCode;

public:
	std::set<GxListener*> m_gxListener;

	void GxListenerAdd(GxListener*);
	void GxListenerDel(GxListener*);

public:
	//通知渲染层的改变
	void RenderDraw(int opc,void* buf,size_t sz,void* p1,void* p2);


};

int GameDirectorMsg(char* buf, size_t sz, void* arg);


#endif // XGame_h__
