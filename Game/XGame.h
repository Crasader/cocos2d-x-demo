#ifndef XGame_h__
#define XGame_h__


#include "XActor.h"
#include "XPlayer.h"
#include "XScene.h"
#include "opcodes.h"
#include "XListener.h"

#define	 X_IDS_AUTH_URL_REQ	"http://mangoschina.blog.163.com/blog/static/27333216120175612634726"
#define  X_IDS_AUTH_URL_PATH "/blog/static/27333216120175612634726"

using namespace xg;

class GxApplication : public Singleton<GxApplication>
{
public:
	time_t m_tiAuthURL; //���һ�λ�õĵ�¼
	string m_strAuthURL;

	GxPlayer m_mySelf;
	GxScene  m_myScene;

	string m_session;  //�Ự����
	string m_password; //����

	string m_game_host; //��Ϸ������������ַ
	int    m_game_port; //��Ϸ�������˿�

protected:

public:
	GxApplication();
	virtual ~GxApplication();

	GxPlayer& MySelf();
	GxScene&  MyScene();

	void LoadConfigFromXmlFile(const char* fname);

	bool AuthUrlIsExpire();
	void AuthUrlSet(std::string str);

	void Login();
	void ConnectGameServer();


public:
	int OnMessage(char* buf, size_t sz, void* arg);

};

int GameDirectorMsg(char* buf, size_t sz, void* arg);


#endif // XGame_h__
