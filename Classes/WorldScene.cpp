#include "WorldScene.h"
#include "SimpleAudioEngine.h"
#include "game/XGame.h"

#include "CommUI.h"

#include "gayola/ByteBuffer.h"
#include "gayola/ByteReader.h"

#include "game/XProtocol.h"

USING_NS_CC;

Scene* GxWorld::createScene()
{
	auto scene = Scene::create();
	auto layer = GxWorld::create();
	layer->setName("main");
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool GxWorld::init()
{
    //////////////////////////////
    // 1. Layer init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
	m_uiLayer = Layer::create();
	addChild(m_uiLayer);
	//m_uiLayer->setPosition(theWinCenter);
 
	m_bUiLogin = true;
	ShowUiLogin(m_bUiLogin);

    return true;
}


void GxWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
 /*   Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif*/
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
	//GxApplication::Instance()->RegisterGuest();


}

void GxWorld::ShowUiLogin(bool& _bVisible)
{
	if (_bVisible)
	{
		Layout* _widget = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ui_login.json"));
		m_uiLayer->addChild(_widget);
		_widget->setName("ui_login.json");

//		Size screenSize = Director::getInstance()->getWinSize();
//		Size rootSize = _widget->getContentSize();

		auto BtnLogin = Helper::seekWidgetByName(_widget, "Button_login");
		if (BtnLogin) {
			BtnLogin->addClickEventListener([=](Ref* sender) 
			{
				string _name;
				auto uname= dynamic_cast<TextField*>( Helper::seekWidgetByName(_widget, "TextField_username"));
				if (uname) {
					_name = uname->getStringValue();
				}

				string _password;
				auto password = dynamic_cast<TextField*>(Helper::seekWidgetByName(_widget, "TextField_password"));
				if (password) {
					_password = password->getStringValue();
				}

				GxApplication::Instance()->LoginUserPassword(_name, _password);

				//m_bUiLogin = false;
				//ShowLoginUi(m_bUiLogin);
								

			});
		}

		//游客登录
		auto BtnLoginGuest = Helper::seekWidgetByName(_widget, "Button_login_guest");
		if (BtnLoginGuest) {
			BtnLoginGuest->addClickEventListener([=](Ref* sender)
			{
				GxApplication::Instance()->LoginGuest();
			});
		}



	}
	else {
		//删除掉
		//m_uiLayer->removeChildByName("ui_login.json");
		SafeRemoveUiByName("ui_login.json");
	}
}

void GxWorld::SafeRemoveUiByName(std::string _name)
{
	auto _widget = m_uiLayer->getChildByName(_name);
	if (_widget) {
		_widget->runAction(Sequence::create(DelayTime::create(0.3f), RemoveSelf::create(), NULL));
	}
}

int GxWorld::OnGxMessage(const char* buf, size_t sz, void* arg)
{
	ByteReader brr(buf,sz);

	uint16_t msgId;
	brr >> msgId;

	if (msgId == XXMSG_LOGIN) {
		int eno = brr.read<int>();
		if (eno == 0) {
			SafeRemoveUiByName("ui_login.json");
			//m_bUiActorSelector = true;
			//ShowUiActorSelector(m_bUiActorSelector);
		}
		else {
			//显示错误信息

		}

		return 1;
	}

	return 0;
}

GxWorld::GxWorld()
{
	GxApplication::Instance()->MsgHandlerAdd(this, 0);
	GxApplication::Instance()->NetMsgHandlerAdd(0, GxWorld::NetMsgHandler, 0, this);
}

GxWorld::~GxWorld()
{
	GxApplication::Instance()->MsgHandlerRemove(this);
	GxApplication::Instance()->NetMsgHandlerRemove(GxWorld::NetMsgHandler);
}

void GxWorld::ShowUiActorSelector(bool& _bVisible)
{
	if (_bVisible)
	{
		XPTO_GAME::c_char_enum();

		Layout* _widget = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ui_actor_selector.json"));
		m_uiLayer->addChild(_widget);
		_widget->setName("ui_actor_selector.json");

		//		Size screenSize = Director::getInstance()->getWinSize();
		//		Size rootSize = _widget->getContentSize();

		auto BtnLogin = Helper::seekWidgetByName(_widget, "Button_random");
		if (BtnLogin) {
			BtnLogin->addClickEventListener([=](Ref* sender)
			{
				//GxApplication::Instance()->CharCreate("<random>");
				XPTO_GAME::c_char_create("<random>");
			});
		}

		auto BtnLoginGuest = Helper::seekWidgetByName(_widget, "Button_enter_game");
		if (BtnLoginGuest) {
			BtnLoginGuest->addClickEventListener([=](Ref* sender)
			{
				XPTO_GAME::c_char_use(GxApplication::Instance()->m_mySelf.m_name);
				SafeRemoveUiByName("ui_actor_selector.json");
			});
		}



	}
	else {
		//删除掉
		SafeRemoveUiByName("ui_actor_selector.json");
	}
}

void GxWorld::UpdateUiForCharEnum()
{
	char buf[256];
	if (!m_bUiActorSelector) return;
	int k = 0;

	Layout* _widget = dynamic_cast<Layout*>(m_uiLayer->getChildByName("ui_actor_selector.json"));
	if (_widget)
	{
		ListView* _lv = dynamic_cast<ListView*>(Helper::seekWidgetByName(_widget, "ListView_main"));
		if (_lv) {
			GxScene& scn = GxApplication::Instance()->MyScene();

			Widget* wt = Helper::seekWidgetByName(_lv, "Panel_item_0");

			k = 0;
			for (auto it : scn.m_childs)
			{
				GxPlayer* ply = dynamic_cast<GxPlayer*>(it);
				if (ply) {
					//
					sprintf(buf, "Panel_item_%d", k);
					Widget* w = Helper::seekWidgetByName(_lv, buf);
					if(w==NULL){
						w = wt->clone();
					}
					cocos2d::ui::Text* labName = dynamic_cast<Text*>(Helper::seekWidgetByName(w, "Label_name"));
					if (labName) {
						labName->setString(ply->m_name);
					}

					//设置点击事件
					w->addClickEventListener([=](Ref* sender)
					{
						//XPTO_GAME::c_char_use(GxApplication::Instance()->m_mySelf.m_name);
						cocos2d::ui::Text* labName = dynamic_cast<Text*>(Helper::seekWidgetByName(w, "Label_name"));
						if(labName) GxApplication::Instance()->m_mySelf.m_name = labName->getStringValue();
						CCLOG("selected %s", GxApplication::Instance()->m_mySelf.m_name.c_str());
					});

				}
				k++;
			}
		}
	}

}


#pragma mark ----网络协议

int GxWorld::NetMsgHandler(const char* buf, size_t sz, void* arg, void* userdata)
{
	GxWorld* _world = (GxWorld*)userdata;
	uint16_t* msgId = (uint16_t*)buf;

	if (*msgId == XSMSG_SESSION)
	{
		CCLOG("token passed");
		_world->m_bUiActorSelector = true;
		_world->ShowUiActorSelector(_world->m_bUiActorSelector);
		return 1;
	}

	if (*msgId == XSMSG_CHAR_ENUM)
	{
		_world->UpdateUiForCharEnum();
		return 1;
	}

	if (*msgId == XSMSG_CHAR_CREATE)
	{

		return 1;
	}

	if (XSMSG_WORLD_NEW == *msgId)
	{
		//界面上显示场景内容 然后告知服务器准备就绪
		XPTO_GAME::c_world_ready();
		return 1;
	}

	return 0;
}
