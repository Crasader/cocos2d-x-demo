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

	int  m_error_code; //´íÎó±àºÅ

private:
//	int m_timeout;

public:
	GxWorld();
	virtual ~GxWorld();

	static cocos2d::Scene* createScene();

    
    CREATE_FUNC(GxWorld);

public:
	virtual bool init();


public:

	void UpdateUiForCharEnum();
	void UpdateUiForCharSelected(Ref* sender);

	void ShowUiErrorShowText(std::string _text, int _timeout=0);
	void ShowUiMessageBox(std::string _text,int _flags);

public:
	void SafeRemoveUiByName(std::string _name);

	void ShowUiLogin();
	void ShowUiActorSelector();
	void ShowUiMain();

public:
	void OnUiRemoveBefore(Ref* sender);
	void OnUiRemoveAfter(Node* sender);


public:
	virtual int OnGxMessage(const char* buf, size_t sz, void* arg);

	static int NetMsgHandler(const char* buf, size_t sz, void* arg, void* userdata);

public:
	virtual void OnMessage(char* buf, size_t sz, void* arg);

};

#endif // __GxWorld_SCENE_H__
