#ifndef __Logo_SCENE_H__
#define __Logo_SCENE_H__


/*

1) 显示公司LOGO
2）显示游戏警告（是否显示过，本地配置文件）
3）获取登录服务器信息
4）读取自己的账号（本地配置文件）

*/


#include "cocos2d.h"

class Logo : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	
	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);
	
	// implement the "static create()" method manually
	CREATE_FUNC(Logo);


	void GetAuthURL(std::string URL);

public:
	virtual void OnMessage(char* buf, size_t sz, void* arg);

};

#endif // __Logo_SCENE_H__
