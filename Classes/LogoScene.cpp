#include "LogoScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* Logo::createScene()
{
	return Logo::create();
}

// on "init" you need to initialize your instance
bool Logo::init()
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
										   CC_CALLBACK_1(Logo::menuCloseCallback, this));
	
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
								origin.y + closeItem->getContentSize().height/2));

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label
	
	auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
	
	// position the label on the center of the screen
	label->setPosition(Vec2(origin.x + visibleSize.width/2,
							origin.y + visibleSize.height - label->getContentSize().height));

	// add the label as a child to this layer
	this->addChild(label, 1);

	// add "Logo" splash screen"
	auto sprite = Sprite::create("Logo.png");

	// position the sprite on the center of the screen
	sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

	// add the sprite as a child to this layer
	this->addChild(sprite, 0);
	

	GetAuthURL("http://mangoschina.blog.163.com/blog/static/27333216120175612634726");


	return true;
}


void Logo::menuCloseCallback(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->end();

	#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
	
	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
	
	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);
	
	
}

void Logo::GetAuthURL(std::string URL)
{

	//链接服务器
	//开启请求并接收线程
	//处理结果在收到结束消息后

	CxHttpClient::ThGet(URL);

}

void Logo::OnAppMessage(char* buf, size_t sz, void* who)
{

}

void Logo::OnNetMessage(char* buf, size_t sz, void* arg)
{

}
