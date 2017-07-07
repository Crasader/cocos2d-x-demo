#ifndef __CommUI_SCENE_H__
#define __CommUI_SCENE_H__


/*

1) ��ʾ��˾CommUI
2����ʾ��Ϸ���棨�Ƿ���ʾ�������������ļ���
3����ȡ��¼��������Ϣ
4����ȡ�Լ����˺ţ����������ļ���

*/


#include "cocos2d.h"
#include "CommUI/XListener.h"

using namespace cocos2d;

class CommUI : public cocos2d::Scene , public GxListener
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void onEnter();

	// a selector callback
	void menuCloseCallback(cocos2d::Ref* pSender);
	void menuConnectCallback(cocos2d::Ref* pSender);
	
	// implement the "static create()" method manually
	CREATE_FUNC(CommUI);


	void GetAuthURL(std::string URL);

public:
	CommUI();
	virtual ~CommUI();

public:
	virtual void OnMessage(char* buf, size_t sz, void* arg);

public:
	//��ʾ�ı������ı�
	virtual void OnChangeDisplyString(const char* txt);

	//����������Ϣ
	virtual void OnError(int ecode, const char* txt);

};

#endif // __CommUI_SCENE_H__
