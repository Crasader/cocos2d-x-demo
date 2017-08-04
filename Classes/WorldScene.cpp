
#ifdef _MSC_VER 
#pragma execution_character_set("utf-8")
#endif

#include "WorldScene.h"
#include "SimpleAudioEngine.h"
#include "game/XGame.h"



#include "gayola/ByteBuffer.h"
#include "gayola/ByteReader.h"

#include "game/XProtocol.h"
#include "MyTcpClient.h"
#include "gayola/XString.h"
#include "CommUI.h"

USING_NS_CC;

#define XX_BTN_TOUCH_EVENT(a) a->addTouchEventListener(CC_CALLBACK_2(GxWorld::touchEvent, this))


Scene* GxWorld::createScene()
{
	auto scene = Scene::create();
	auto layer = GxWorld::create();
	layer->setName("main");
	scene->addChild(layer);
	return scene;
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
 
	//m_bUiLogin = true;
	//ShowUiLogin();
	//m_bUiWarning = true;
	//ShowUiWarning();

    return true;
}



void GxWorld::onEnter()
{
	Layer::onEnter();

	ShowUiLogo();

}

void GxWorld::ShowUiError()
{
	char buf[256];
	std::string jsonfile = "ui_error.json";

	if (m_bUiError == false) {
		SafeRemoveUiByName(jsonfile);
		return;
	}

	Layout* _widget = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(jsonfile.c_str()));
	m_uiLayer->addChild(_widget);
	_widget->setName(jsonfile);
	_widget->setZOrder(128);


	//
	auto _show_list= (Helper::seekWidgetByName(_widget, "Panel_show_text"));
	if (_show_list && m_error_win_flag & XUI_ERROR_NAM)
	{

		//显示错误信息 Label_text_main
		cocos2d::ui::Text* labName = dynamic_cast<Text*>(Helper::seekWidgetByName(_widget, "Label_text_main"));
		if (labName) {
			labName->setString(GxApplication::Instance()->ErrorLastString());
		}

		int k = 0;
		//显示最近的其他错误信息
		for (int i = 0; i < 5; i++)
		{
			sprintf(buf, "Label_error_%d", i);
			cocos2d::ui::Text* labName = dynamic_cast<Text*>(Helper::seekWidgetByName(_widget, buf));
			if (labName) {
				std::string str = GxApplication::Instance()->ErrorString(i + 1);
				if (str.empty()) {
					labName->setVisible(false);
				}
				else {
					labName->setVisible(true);
					labName->setString(str);
					k++;
				}
			}
		}

		Widget* w = (Helper::seekWidgetByName(_widget, "ListView_main"));
		if (w) w->setVisible(k > 0);
	}
	else if(_show_list) {
		_show_list->setVisible(false);
	}


	//显示控制按钮 
	auto btn_bar= (Helper::seekWidgetByName(_widget, "Panel_button"));
	if(btn_bar) btn_bar->setVisible(m_error_win_flag & XUI_ERROR_BTN);

	//显示提示条 
	Widget* _barShow= (Helper::seekWidgetByName(_widget, "Panel_bar"));
	if (_barShow)
	{
		_barShow->setVisible(m_error_win_flag & XUI_ERROR_BAR);

		//Label_text_main_in_bar
		Text* _txt=dynamic_cast<Text*> (Helper::seekWidgetByName(_widget, "Label_text_main_in_bar"));
		if (_txt) {
			_txt->setString(GxApplication::Instance()->ErrorLastString());
		}

		//Panel_main
		auto wc= (Helper::seekWidgetByName(_widget, "Panel_main"));
		if (wc) {
			wc->addClickEventListener([=](Ref* sender)
			{
				m_bUiError = false;
				SafeRemoveUiByName(jsonfile);

				if (m_bUiLogin) {
					UiSetFocused("ui_login.json", "TextField_password");
				}

			});
		}

	}

}

void GxWorld::ShowUiWarning()
{
	char buf[256];
	std::string jsonfile = "ui_warning.json";

	if (m_bUiWarning == false) {
		SafeRemoveUiByName(jsonfile);
		return;
	}

	Layout* _widget = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(jsonfile.c_str()));
	m_uiLayer->addChild(_widget);
	_widget->setName(jsonfile);

	//Label_text
	Text* labText=dynamic_cast<Text*>(Helper::seekWidgetByName(_widget, "Label_text"));
	if (labText) {
		labText->setString(XLanguage::Instance()->GetValue("IDS_ERROR_LOGIN_U_P"));
	}

	auto Btn1 = Helper::seekWidgetByName(_widget, "Button_start");
	if (Btn1) {
		XX_BTN_TOUCH_EVENT(Btn1);
		Btn1->addClickEventListener([=](Ref* sender)
		{
			//XPTO_GAME::c_char_use(GxApplication::Instance()->m_mySelf.m_name);
			m_bUiLogin = true;
			m_bUiWarning = false;
			SafeRemoveUiByName(jsonfile);
			ShowUiLogin();
		});
	}

}


void GxWorld::ShowUiLogin()
{
	if (m_bUiLogin)
	{
		Layout* _widget = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ui_login.json"));
		m_uiLayer->addChild(_widget);
		_widget->setName("ui_login.json");

//		Size screenSize = Director::getInstance()->getWinSize();
//		Size rootSize = _widget->getContentSize();

		auto BtnLogin = Helper::seekWidgetByName(_widget, "Button_login");
		if (BtnLogin) {

			BtnLogin->addTouchEventListener(CC_CALLBACK_2(GxWorld::touchEvent, this));

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
			XX_BTN_TOUCH_EVENT(BtnLoginGuest);
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

void GxWorld::ShowUiActorSelector()
{
	GxApplication* app= GxApplication::Instance();

	if (m_bUiActorSelector)
	{
		//清除旧的列表数据
		GxApplication::Scene()->clear();

		XPTO_GAME::c_char_enum();

		Layout* _widget = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ui_actor_selector.json"));
		m_uiLayer->addChild(_widget);
		_widget->setName("ui_actor_selector.json");

		//		Size screenSize = Director::getInstance()->getWinSize();
		//		Size rootSize = _widget->getContentSize();


		//Label_account_id
		Text* _acct_id=dynamic_cast<Text*>(Helper::seekWidgetByName(_widget, "Label_account_id"));
		if (_acct_id) {
			string str = app->m_username + "[" + app->m_acct_id+"]";
			_acct_id->setString(str);
		}


		auto BtnLogin = Helper::seekWidgetByName(_widget, "Button_random");
		if (BtnLogin) {
			XX_BTN_TOUCH_EVENT(BtnLogin);
			BtnLogin->addClickEventListener([=](Ref* sender)
			{
				//GxApplication::Instance()->CharCreate("<random>");
				XPTO_GAME::c_char_create("<random>");
			});
		}

		auto BtnLoginGuest = Helper::seekWidgetByName(_widget, "Button_enter_game");
		if (BtnLoginGuest) {
			XX_BTN_TOUCH_EVENT(BtnLoginGuest);
			BtnLoginGuest->addClickEventListener([=](Ref* sender)
			{
				XPTO_GAME::c_char_use(GxApplication::Instance()->m_mySelf.m_name);
				m_bUiActorSelector = false;
				SafeRemoveUiByName("ui_actor_selector.json");
			});
		}


		auto ChkSaveSelected = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(_widget, "CheckBox_save_actor_selected"));
		if (ChkSaveSelected) {
			ChkSaveSelected->addClickEventListener([=](Ref* sender)
			{
				GxApplication* app = GxApplication::Instance();
				//ChkSaveSelected->setSelected(!ChkSaveSelected->getSelectedState());
				bool _b = !ChkSaveSelected->getSelectedState();
				app->AttribSet("ActorSaveSelected", XOBJ_ATTR_TYPE::OAT_I32, (const char*)&_b, 4);
				if (app->Self()->m_name.length()) {
					app->AttribSet("ActorSelected", XOBJ_ATTR_TYPE::OAT_STRING,
						app->Self()->m_name.c_str(), app->Self()->m_name.length());
				}
				CCLOG("selected:%s,save:%d", app->Self()->m_name.c_str(),_b);
			});
		}


	}
	else {
		//删除掉
		SafeRemoveUiByName("ui_actor_selector.json");
	}
}


void GxWorld::OnUiRemoveBefore(Ref* sender)
{

}

void GxWorld::OnUiRemoveAfter(Node* sender)
{
	if (sender) {
		if (sender->getName().compare("ui_logo.json") == 0) {
			//ActionManagerEx::getInstance()->releaseActions();
			ActionManagerEx::getInstance()->releaseActionByName("ui_logo.json", "LogoShow");
		}
	}
}

void GxWorld::SafeRemoveUiByName(std::string _name)
{
	auto _widget = m_uiLayer->getChildByName(_name);
	if (_widget) {
		OnUiRemoveBefore(_widget);
		_widget->runAction(Sequence::create(DelayTime::create(0.1f),
			CallFuncN::create(this,CC_CALLFUNCN_SELECTOR(GxWorld::OnUiRemoveAfter)),
			RemoveSelf::create(), NULL));
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
			m_error_win_flag = 0;
			m_error_win_flag |= XUI_ERROR_BAR;
			std::string str = XLanguage::Instance()->GetValue("IDS_ERROR_LOGIN_U_P");
			GxApplication::Instance()->ErrorPushBack(eno, str.c_str());
			m_bUiError = true;
			ShowUiError();
		}

		return 1;
	}

	return 0;
}

#define XUI_TEXT_SET_U64(a,b) \
	auto lab = dynamic_cast<Text*>(Helper::seekWidgetByName(_widget, a)); \
	if (lab) { \
		uint64_t v = ply->AttribGetValueReal<uint64_t>(b); \
		std::string strValue = std::to_string(v); \
		lab->setString(strValue); }


void GxWorld::UpdateUiForMySelfInfo()
{
	string strValue;
	uint64_t v = 0;
	GxPlayer* ply = GxApplication::Self();

	//查找主界面
	Widget* _widget = dynamic_cast<Widget*>(m_uiLayer->getChildByName("ui_main.json"));
	if (_widget) {
		auto labCoin = dynamic_cast<Text*>(Helper::seekWidgetByName(_widget, "Label_my_coin"));
		if (labCoin) {
			v = ply->AttribGetValueReal<uint64_t>("coin");
			strValue = std::to_string(v);
			labCoin->setString(strValue);
		}

		//auto labGold = dynamic_cast<Text*>(Helper::seekWidgetByName(_widget, "Label_my_gold"));
		//if (labGold) {
		//	v = ply->AttribGetValueReal<uint64_t>("gold");
		//	strValue = std::to_string(v);
		//	labGold->setString(strValue);
		//}
		XUI_TEXT_SET_U64("Label_my_gold", "gold")

	}

}

void GxWorld::UpdateUiForCharSelected(Ref* sender)
{
	char buf[256];
	Layout* _widget = dynamic_cast<Layout*>(m_uiLayer->getChildByName("ui_actor_selector.json"));
	if (_widget)
	{
		int k = 0;
		while (1) 
		{
			sprintf(buf, "Panel_item_%d", k);
			Widget* w = Helper::seekWidgetByName(_widget, buf);
			k++;

			if (w == NULL) break;
			
			Widget* mark= Helper::seekWidgetByName(w, "ImageView_smark");
			bool b = false;
			if (w == sender) {
				Text* _name = dynamic_cast<Text*>(Helper::seekWidgetByName(w, "Label_name"));
				if(_name) GxApplication::Instance()->Self()->m_name = _name->getString();
				b = true;
			}
			if (mark) mark->setVisible(b);
		}
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
			if (scn.m_childs.size() == 0) {
				_lv->setVisible(false);
				return;
			}
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
						_lv->addChild(w);
						w->setName(buf);
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
						//ImageView_smark
						//其他条目选择都隐藏 这条显示
						UpdateUiForCharSelected(sender);
					});

				}
				k++;
			}
			UpdateUiForCharSelected(wt);
			//设置ListView尺寸
			Size sz = _lv->getInnerContainerSize();
			sz.height = wt->getSize().height*k;
			_lv->setInnerContainerSize(sz);
			_lv->setVisible(k > 0);
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
		_world->ShowUiActorSelector();
		return 1;
	}

	if (*msgId == XSMSG_CHAR_ENUM)
	{
		_world->UpdateUiForCharEnum();
		return 1;
	}


	if (XSMSG_WORLD_NEW == *msgId)
	{
		_world->m_bUiMain = true;
		_world->ShowUiMain();

		//界面上显示场景内容 然后告知服务器准备就绪
		XPTO_GAME::c_world_ready();
		return 1;
	}

	if (XSMSG_CHAR_CREATE == *msgId)
	{
		XPTO_GAME::c_char_enum();
		int* err = (int*)(buf+2);
		if (*err == XEC_OK) {
			//创建角色成功
			//XPTO_GAME::c_char_enum();
		}
		return 1;
	}

	if (XSMSG_CHAR_RENAME == *msgId)
	{
		int* err = (int*)(buf + 2);
		if (*err == 0) {
			//改名成功
			//_world->ShowUiErrorShowText("改名成功", 5);
		}
		else {
			//名字不被接受 重新启名
			//_world->ShowUiErrorShowText("名字不被接受 重新启名",0);
		}
		return 1;
	}

	if (XSMSG_ACCOUNT_USE_OTHER == *msgId)
	{
		GxApplication::Instance()->ErrorPushBack(XEC_C_NET_DISCONN, "账号在其他地方登录，此端将被断开");
		_world->m_bUiError = true;
		_world->ShowUiError();
		return 1;
	}

	if (XSMSG_MY_INFO == *msgId)
	{
		_world->UpdateUiForMySelfInfo();
		return 1;
	}



	return 0;
}

//void GxWorld::ShowUiErrorShowText(std::string _text, int _timeout)
//{
//
//}

void GxWorld::OnMessage(char* buf, size_t sz, void* arg)
{
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

			//设置协议

		}

		if (state_name == XTCS_DISCONNECT)
		{
			//界面显示网络断开 提示重新连接
			GxApplication::Instance()->ErrorPushBack(XEC_C_NET_DISCONN,"网络断开 重新连接");
			m_bUiError = true;
			ShowUiError();
		}

		return;
	}
}

/**
显示游戏中的主界面
*/
void GxWorld::ShowUiMain()
{

	if (m_bUiMain == false) {
		SafeRemoveUiByName("ui_main.json");
		return;
	}

		

		Layout* _widget = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("ui_main.json"));
		m_uiLayer->addChild(_widget);
		_widget->setName("ui_main.json");

		//显示角色名字 金币 硬币 信息
		cocos2d::ui::Text* labName = dynamic_cast<Text*>(Helper::seekWidgetByName(_widget, "Label_my_name"));
		if (labName) {
			labName->setString(GxApplication::Self()->m_name);
		}

		auto BtnLogin = Helper::seekWidgetByName(_widget, "Button_back_login");
		if (BtnLogin) {
			BtnLogin->addClickEventListener([=](Ref* sender)
			{
				//GxApplication::Instance()->CharCreate("<random>");
				m_bUiLogin = true;
				ShowUiLogin();
				m_bUiMain = false;
				SafeRemoveUiByName("ui_main.json");
			});
		}

		//auto BtnLoginGuest = Helper::seekWidgetByName(_widget, "Button_enter_game");
		//if (BtnLoginGuest) {
		//	BtnLoginGuest->addClickEventListener([=](Ref* sender)
		//	{
		//		XPTO_GAME::c_char_use(GxApplication::Instance()->m_mySelf.m_name);
		//		SafeRemoveUiByName("ui_actor_selector.json");
		//	});
		//}



	
}

void GxWorld::touchEvent(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	float _s = 1.0;
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
	{
		_s = 0.8f;
	}
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		break;
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}

	Node* n = dynamic_cast<Node*>(sender);
	if (n) {
		n->setScale(_s);
	}

}

void GxWorld::UiSetFocused(std::string jsonfile, std::string componet, bool _val/*=true*/)
{
	Widget* _widget = dynamic_cast<Widget*>(m_uiLayer->getChildByName(jsonfile));
	if (_widget) {
		auto wc= (Helper::seekWidgetByName(_widget, componet));
		if (wc) wc->setFocused(_val);
	}
}

void GxWorld::ShowUiRename()
{

}

void GxWorld::ShowUiLogo()
{
	string jsonfname = "ui_logo.json";

	Layout* _widget = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(jsonfname.c_str()));
	addChild(_widget);
	_widget->setName(jsonfname);
	ActionObject* ao= ActionManagerEx::getInstance()->getActionByName(jsonfname.c_str(),"LogoShow" );
	if (ao) {
		ao->play(CallFunc::create(CC_CALLBACK_0(GxWorld::OnLogoShowAfter,this)));
	}
}

void GxWorld::OnLogoShowAfter()
{
	Node* n = getChildByName("ui_logo.json");
	if (n) {
		n->runAction(Sequence::create(DelayTime::create(0.1f),
			CallFuncN::create(this, CC_CALLFUNCN_SELECTOR(GxWorld::OnUiRemoveAfter)),
			RemoveSelf::create(), NULL));
	}

	m_bUiWarning = true;
	ShowUiWarning();
}
