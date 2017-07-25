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
	time_t m_tiAuthURL; //���һ�λ�õĵ�¼
	string m_strAuthURL;

	GxPlayer m_mySelf;
	GxScene  m_myScene;

	string m_acct_id;
	string m_username;	//�˺���
	string m_session;  //�Ự����
	string m_password; //����

	string m_game_host; //��Ϸ������������ַ
	int    m_game_port; //��Ϸ�������˿�

	string m_login_host; //��¼��������ַ
	int	   m_login_port; //��¼�������˿�

	int m_net_pcode;	//�ͻ���Э��汾��

	string m_strCfgFilename;


	std::list<gxmsginfo_t> msgHandlers;

	void LoginGuest();
public:


	void ConfigDefaultSave(std::string _filename);
public:
	GxBagClient m_bagClient;	//�����ͻ���
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


	//ע��һ���ο� Ҳ�Ǹ����һ��Ĭ�ϵ��˺�
	void RegisterGuest();

	void OnAfterConnectGame();

	void ResponseSessionRecvAfter();

	void CharEnum();
	void CharCreate();
	void CharCreateAuto();
	void CharUse(std::string charName);

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

public:
	std::set<GxListener*> m_gxListener;

	void GxListenerAdd(GxListener*);
	void GxListenerDel(GxListener*);

public:
	//֪ͨ��Ⱦ��ĸı�
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

};

int GameDirectorMsg(char* buf, size_t sz, void* arg);


#endif // XGame_h__
