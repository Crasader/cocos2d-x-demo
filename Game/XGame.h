#ifndef XGame_h__
#define XGame_h__


#include "XActor.h"
#include "XPlayer.h"
#include "XScene.h"

using namespace xg;

class GxApplication : public Singleton<GxApplication>
{
public:
	time_t m_tiAuthURL; //最后一次获得的登录
	string m_strAuthURL;

	GxPlayer m_mySelf;

	string m_session;  //会话令牌
	string m_password; //密码

protected:

public:
	GxApplication();
	virtual ~GxApplication();

	GxPlayer& MySelf();

	void LoadConfigFromXmlFile(const char* fname);

	bool AuthUrlIsExpire();
	void AuthUrlSet(std::string str);



};



#endif // XGame_h__
