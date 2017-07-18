#include "HallScene.h"
#include "SimpleAudioEngine.h"

#include "gayola/CxHttpClient.h"
#include "MyTcpClient.h"

#include "game/XGame.h"
#include "gayola/ByteBuffer.h"

#include "CommUI.h"

#include <fstream>

USING_NS_CC;

Scene* Hall::createScene()
{
	auto scene = Scene::create();
	auto layer = Hall::create();
	layer->setName("main");
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool Hall::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !Layer::init() )
	{
		return false;
	}
	
	//xg_warning_start.json
	XUiLayout* _Layout = NULL;

	while (_Layout == NULL)
		_Layout = XUiLayout::createWith(static_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("xg_hall.json")));


	//Label_agree_text_Copy0
	//	_Layout->SetDefaultWidget("Panel_default", CC_CALLBACK_1(Logo::ShowExitGameMsgBox, this));

	_Layout->setContentSize(theWinSize);
	this->addChild(_Layout);
	_Layout->setName("xg_hall.json");



	return true;
}



void Hall::OnMessage(char* buf, size_t sz, void* arg)
{


}

void Hall::OnChangeDisplyString(const char* txt)
{
	auto label = Label::create(txt, "微软雅黑", 24);
	if (label) {
		label->setPosition(Vec2(480,320));
		this->addChild(label, 1);
		label->setName("main_label");
	}
}

void Hall::OnError(int ecode, const char* txt)
{

}

Hall::Hall()
{
	GxApplication::Instance()->GxListenerAdd(this);
}

Hall::~Hall()
{
	GxApplication::Instance()->GxListenerDel(this);
}


void Hall::onEnter()
{
	Layer::onEnter();

	//动态播放动画 后台获取登录验证

}
