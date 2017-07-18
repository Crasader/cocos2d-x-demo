#ifndef __Hall_SCENE_H__
#define __Hall_SCENE_H__


/*

1) ��ʾ��˾Hall
2����ʾ��Ϸ���棨�Ƿ���ʾ�������������ļ���
3����ȡ��¼��������Ϣ
4����ȡ�Լ����˺ţ����������ļ���

*/


#include "cocos2d.h"
#include "game/XListener.h"

using namespace cocos2d;

class Hall : public cocos2d::Layer, public GxListener
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	virtual void onEnter();

	
	// implement the "static create()" method manually
	CREATE_FUNC(Hall);


public:
	Hall();
	virtual ~Hall();

public:
	virtual void OnMessage(char* buf, size_t sz, void* arg);

public:
	//��ʾ�ı������ı�
	virtual void OnChangeDisplyString(const char* txt);

	//����������Ϣ
	virtual void OnError(int ecode, const char* txt);

};

#endif // __Hall_SCENE_H__
