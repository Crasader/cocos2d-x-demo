#ifndef __Logo_SCENE_H__
#define __Logo_SCENE_H__


/*

1) ��ʾ��˾LOGO
2����ʾ��Ϸ���棨�Ƿ���ʾ�������������ļ���
3����ȡ��¼��������Ϣ
4����ȡ�Լ����˺ţ����������ļ���

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


	void OnAfterLogoShow();

	void ShowWarning();
	void OnWarningAgree();

public:
	virtual void OnMessage(char* buf, size_t sz, void* arg);

public:
	//��ʾ�ı������ı�
	virtual void OnChangeDisplyString(const char* txt);

	//����������Ϣ
	virtual void OnError(int ecode, const char* txt);

private:
	bool _bShowWarning; //��ʾ����
	
};

#endif // __Logo_SCENE_H__
