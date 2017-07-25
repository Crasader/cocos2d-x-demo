#ifndef __GxWorld_SCENE_H__
#define __GxWorld_SCENE_H__

#include "cocos2d.h"
#include <string>

#include "Game/XMsgHandler.h"

using namespace cocos2d;
using namespace std;

class GxWorld : public cocos2d::Layer,public GxMsgHandler
{
protected:
	bool m_bUiLogin;
	bool m_bUiActorSelector;

	Layer* m_uiLayer;

public:
	GxWorld();
	virtual ~GxWorld();


    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(GxWorld);

public:
	void ShowTestUi(bool& _bVisible);
	void ShowUiLogin(bool& _bVisible);
	void ShowUiActorSelector(bool& _bVisible);

	void SafeRemoveUiByName(std::string _name);


public:
	virtual int OnGxMessage(const char* buf, size_t sz, void* arg);

};

#endif // __GxWorld_SCENE_H__
