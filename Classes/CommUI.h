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


#endif // __CommUI_SCENE_H__
