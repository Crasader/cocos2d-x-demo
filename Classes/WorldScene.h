#ifndef __GxWorld_SCENE_H__
#define __GxWorld_SCENE_H__

#include "cocos2d.h"
#include <string>

#include "Game/XMsgHandler.h"
#include "game/XListener.h"

using namespace cocos2d;
using namespace std;

class GxWorld : public cocos2d::Layer,public GxMsgHandler, public GxListener
{
protected:
	bool m_bUiLogin;
	bool m_bUiActorSelector;
	bool m_bUiMain;

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

	void UpdateUiForCharEnum();
	void ShowUiErrorShowText(std::string _text, int _timeout);
public:
	void ShowTestUi(bool& _bVisible);
	void ShowUiLogin();
	void ShowUiActorSelector();
	void ShowUiMain();

	void SafeRemoveUiByName(std::string _name);


public:
	virtual int OnGxMessage(const char* buf, size_t sz, void* arg);

	static int NetMsgHandler(const char* buf, size_t sz, void* arg, void* userdata);

public:
	virtual void OnMessage(char* buf, size_t sz, void* arg);

};

#endif // __GxWorld_SCENE_H__
