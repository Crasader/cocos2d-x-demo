#ifndef XGame_h__
#define XGame_h__


#ifdef _MSC_VER 
#pragma execution_character_set("utf-8")
#endif
/*



*/


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
#include <stdarg.h>
#include <unordered_map>

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
	time_t m_tiAuthURL; //���һ�λ�õĵ�¼
	string m_strAuthURL;


	string m_acct_id;	//�˺� Ψһ��
	string m_username;	//�˺���
	string m_session;  //�Ự����
	string m_password; //����

	string m_game_host; //��Ϸ������������ַ
	int    m_game_port; //��Ϸ�������˿�

	string m_login_host; //��¼��������ַ
	int	   m_login_port; //��¼�������˿�

	int m_net_pcode;	//�ͻ���Э��汾��

	string m_strCfgFilename;

	int m_iLastError;

	int m_iTimeDelta;

	string m_write_path_root;

	GxPlayer m_mySelf;
	GxScene  m_myScene;
	GxBagClient m_bagClient;	//�����ͻ���

	std::vector<gx_error_t> m_strErrorCnt;

public:

	static GxScene* Scene();
	static GxPlayer* Self();

	static GXLOG gLog;


	void ConfigDefaultSave(std::string _filename);
public:

	tinyxml2::XMLDocument m_cfgDoc;

	std::string GetValueStringFrom(tinyxml2::XMLElement* _elm, std::string kname);

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
	void LoginGuest();


	//ע��һ���ο� Ҳ�Ǹ����һ��Ĭ�ϵ��˺�
	void RegisterGuest();
	void OnAfterConnectGame();
	void ResponseSessionRecvAfter();


	void LoginAuto(void*);
	void Login(std::string _url);
	//�յ���¼������Ϣ��
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
	//֪ͨ��Ⱦ��ĸı�
	void RenderDraw(int opc,void* buf,size_t sz,void* p1,void* p2);
	void AgreeWarning(bool _agree, bool _save);

	void SaveUserPwdSidToCfg();
	void LoginUserPassword(string _name, string _password);


public: //������Ϣ
	std::list<gxmsginfo_t> msgHandlers;
	void MsgHandlerAdd(GxMsgHandler* _handler,int _order);
	void MsgHandlerRemove(GxMsgHandler* _handler);
	void MsgHandlerSort();
	void MsgHandlerDespatch(const void* buf,size_t sz,void* arg);

public: //������Ϣ

	std::map<uint16_t, std::list<gx_net_msg_handler_t>> mapNetHandler;

	/**
	����������Ϣ
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
