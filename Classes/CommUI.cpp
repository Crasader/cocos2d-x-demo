#include "CommUIScene.h"
#include "SimpleAudioEngine.h"

#include "gayola/CxHttpClient.h"
#include "MyTcpClient.h"

#include "CommUI/XCommUI.h"
#include "gayola/ByteBuffer.h"

#include <fstream>

USING_NS_CC;

Scene* CommUI::createScene()
{
	auto scene = Scene::create();
	auto layer = CommUI::create();
	layer->setName("main");
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool CommUI::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !Scene::init() )
	{
		return false;
	}
	
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
										   "CloseNormal.png",
										   "CloseSelected.png",
										   CC_CALLBACK_1(CommUI::menuCloseCallback, this));
	
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
								origin.y + closeItem->getContentSize().height/2));


	auto connectItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(CommUI::menuConnectCallback, this));


	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, connectItem, NULL);
	menu->setPosition(Vec2(480,320));
	menu->alignItemsHorizontally();
	this->addChild(menu, 1);



	//GetAuthURL("http://mangoschina.blog.163.com/blog/static/27333216120175612634726");


	return true;
}


void CommUI::menuCloseCallback(Ref* pSender)
{

	GetAuthURL("http://mangoschina.blog.163.com/blog/static/27333216120175612634726");
	
}

void CommUI::GetAuthURL(std::string URL)
{

	//���ӷ�����
	//�������󲢽����߳�
	//���������յ�������Ϣ��

	CxHttpClient::ThGet(URL, XzAppMessagePushBack,this);

}


void CommUI::OnMessage(char* buf, size_t sz, void* arg)
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

void CommUI::OnChangeDisplyString(const char* txt)
{
	auto label = Label::create(txt, "΢���ź�", 24);
	if (label) {
		label->setPosition(Vec2(480,320));
		this->addChild(label, 1);
		label->setName("main_label");
	}
}

void CommUI::OnError(int ecode, const char* txt)
{

}

CommUI::CommUI()
{
	GxApplication::Instance()->GxListenerAdd(this);
}

CommUI::~CommUI()
{
	GxApplication::Instance()->GxListenerDel(this);
}

void CommUI::menuConnectCallback(cocos2d::Ref* pSender)
{
	CCTcpClient::shared()->Connect("127.0.0.1", 4002);
}

void CommUI::onEnter()
{
	Scene::onEnter();

	//��̬���Ŷ��� ��̨��ȡ��¼��֤

}
