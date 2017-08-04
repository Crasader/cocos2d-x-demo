#ifndef __CommUI_SCENE_H__
#define __CommUI_SCENE_H__


/*


*/


#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;
using namespace CocosDenshion;

extern Vec2 theUICenter;
extern Size theWinSize;
extern Vec2 theWinCenter;

void XUIInit();


class XUiLayout : public Layout
{
public:
	XUiLayout();
	~XUiLayout();

	static XUiLayout* createWith(Layout* _layout);


	virtual bool isKeyboardEnabled() const { return _keyboardEnabled; };
	virtual void setKeyboardEnabled(bool value);


	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* unused_event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* unused_event);


	void SetDefaultWidget(std::string _name, ccWidgetClickCallback _callback);

protected:
	Widget* m_defaultWidget;

	ccWidgetClickCallback _clickEventListenerDefault;

	bool _keyboardEnabled;
	EventListenerKeyboard* _keyboardListener;

};

#define xui_widget_click(a,b) _w = (Helper::seekWidgetByName(_Layout, a)); if (_w){ _w->setVisible(true);  \
	_w->addClickEventListener(CC_CALLBACK_1(b, this)); _w->setTouchEnabled(true);  }

#define xui_widget_click_ex(a,b,c) _w = (Helper::seekWidgetByName(a, b)); if (_w){ _w->setVisible(true);  \
	_w->addClickEventListener(CC_CALLBACK_1(c, this)); _w->setTouchEnabled(true);  }



#define XUI_ERROR_BAR  0x0002
#define XUI_ERROR_BTN  0x0004
#define XUI_ERROR_NAM  0x0008



#endif // __CommUI_SCENE_H__
