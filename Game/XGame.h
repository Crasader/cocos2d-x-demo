#ifndef XGame_h__
#define XGame_h__


#include "XActor.h"
#include "XPlayer.h"
#include "XScene.h"

using namespace xg;

class GxApplication : public Singleton<GxApplication>
{
public:
	time_t m_tiAuthURL; //���һ�λ�õĵ�¼
	string m_strAuthURL;

	GxPlayer m_mySelf;

	string m_session;  //�Ự����
	string m_password; //����

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
