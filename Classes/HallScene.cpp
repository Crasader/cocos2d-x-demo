#include "HallScene.h"
#include "SimpleAudioEngine.h"

#include "gayola/CxHttpClient.h"
#include "MyTcpClient.h"

#include "game/XGame.h"
#include "gayola/ByteBuffer.h"

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
	


	return true;
}




void Hall::OnMessage(char* buf, size_t sz, void* arg)
{

#if(0)
	auto label = Label::createWithTTF(buf, "fonts/Marker Felt.ttf", 24);
	label->setPosition(Vec2(480, 320));
	this->addChild(label, 1);
#endif


#if(0)
	std::ofstream ofs;
	ofs.open("d:\\1.txt", std::ios_base::binary);
	if (ofs.good()) {
		ofs.write(buf, sz);
		ofs.close();
	}
#endif

	ByteReader bbr(buf, sz);
	uint16_t msgId;
	bbr >> msgId;

	if (msgId == XXMSG_TCP_EVENT)
	{
		int state_name;
		int state_val;
		int arg;
		bbr >> state_name;
		bbr >> state_val;
		bbr >> arg;

		if (state_name == XTCS_CONNECT && arg == 1)
		{
			OnChangeDisplyString("connected.");
		}
		return;
	}


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
