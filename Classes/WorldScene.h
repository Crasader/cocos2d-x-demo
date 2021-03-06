#ifndef __GxWorld_SCENE_H__
#define __GxWorld_SCENE_H__

#include "cocos2d.h"
#include <string>

#include "Game/XGame.h"
#include "Game/XMsgHandler.h"
#include "game/XListener.h"
#include "CommUI.h"

using namespace cocos2d;
using namespace std;

class GxWorld : public cocos2d::Layer,public GxMsgHandler, public GxListener
{
protected:
	bool m_bUiLogin;
	bool m_bUiActorSelector;
	bool m_bUiMain;
	bool m_bUiError;
	bool m_bUiWarning;
	bool m_bUiRename;
	bool m_bUiOption; //显示游戏选项 调声音的窗口
	bool m_bUiAutoAction; //是否无视自动执行界面功能

	Layer* m_uiLayer;

	int  m_error_win_flag;
	int  m_error_code; //错误编号
	DWORD  m_flag_error_button; //错误窗口的按钮显示标志
	int  m_win_result;//界面返回结果

private:
//	int m_timeout;
	GxApplication* _app;

public:
	GxWorld();
	virtual ~GxWorld();

	static cocos2d::Scene* createScene();

    
    CREATE_FUNC(GxWorld);

public:
	virtual bool init();
	virtual void onEnter();

public:

	void UpdateUiForCharEnum();
	void UpdateUiForCharSelected(Ref* sender);
	void UpdateUiForMySelfInfo();

//	void ShowUiErrorShowText(std::string _text, int _timeout=0);
//	void ShowUiMessageBox(std::string _text,int _flags);
	void UiSetFocused(std::string jsonfile,std::string componet,bool _val=true);

	void SafeRemoveUiByName(std::string _name);

	void ShowUiLogo();
	void ShowUiWarning();
	void ShowUiLogin();
	void ShowUiActorSelector();
	void ShowUiError();
	void ShowUiRename();

	void ShowOption();

	void ShowUiMain();

	void OnUiRemoveBefore(Ref* sender);
	void OnUiRemoveAfter(Node* sender);

	void OnLogoShowAfter();

	void touchEvent(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);


public:
	virtual int OnGxMessage(const char* buf, size_t sz, void* arg);

	static int NetMsgHandler(const char* buf, size_t sz, void* arg, void* userdata);
	
	/**
	处理角色相关的协议
	*/
	static int NetMsgCharHandler(const char* buf, size_t sz, void* arg, void* userdata);

public:
	virtual void OnMessage(char* buf, size_t sz, void* arg);

};

#endif // __GxWorld_SCENE_H__
