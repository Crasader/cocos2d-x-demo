#ifndef __Hall_SCENE_H__
#define __Hall_SCENE_H__


/*

1) 显示公司Hall
2）显示游戏警告（是否显示过，本地配置文件）
3）获取登录服务器信息
4）读取自己的账号（本地配置文件）

*/


#include "cocos2d.h"
#include "game/XListener.h"

using namespace cocos2d;

class Hall : public cocos2d::Scene , public GxListener
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void onEnter();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);
	void menuConnectCallback(cocos2d::Ref* pSender);
	
	// implement the "static create()" method manually
	CREATE_FUNC(Hall);


	void GetAuthURL(std::string URL);

public:
	Hall();
	virtual ~Hall();

public:
	virtual void OnMessage(char* buf, size_t sz, void* arg);

public:
	//显示文本发生改变
	virtual void OnChangeDisplyString(const char* txt);

	//发生错误信息
	virtual void OnError(int ecode, const char* txt);

};

#endif // __Hall_SCENE_H__
