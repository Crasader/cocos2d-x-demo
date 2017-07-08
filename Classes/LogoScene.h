#ifndef __Logo_SCENE_H__
#define __Logo_SCENE_H__


/*

1) 显示公司LOGO
2）显示游戏警告（是否显示过，本地配置文件）
3）获取登录服务器信息
4）读取自己的账号（本地配置文件）

*/


#include "cocos2d.h"
#include "game/XListener.h"

using namespace cocos2d;

class Logo : public cocos2d::Scene , public GxListener
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void onEnter();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);
	void menuConnectCallback(cocos2d::Ref* pSender);
	
	// implement the "static create()" method manually
	CREATE_FUNC(Logo);


	void GetAuthURL(std::string URL);

public:
	Logo();
	virtual ~Logo();

	void ShowLogo();
	void OnAfterLogoShow();

	void ShowWarning();
	void OnWarningAgree();
	void OnAfterWarningAgree();

public:
	virtual void OnMessage(char* buf, size_t sz, void* arg);

public:
	//显示文本发生改变
	virtual void OnChangeDisplyString(const char* txt);

	//发生错误信息
	virtual void OnError(int ecode, const char* txt);

private:
	bool _bShowWarning; //显示警告
	
};

#endif // __Logo_SCENE_H__
