
#include "CommUI.h"


Vec2 theUICenter;

void XUIInit()
{
	cocos2d::Size sz = Director::getInstance()->getVisibleSize();
	theUICenter.x = sz.width / 2;
	theUICenter.y = sz.height / 2;
}

///////////////////////////////////////////////////////////////////

XUiLayout::XUiLayout()
{
	_keyboardEnabled = false;
	_keyboardListener = nullptr;
	m_defaultWidget = nullptr;
	_clickEventListenerDefault = nullptr;
}


XUiLayout::~XUiLayout()
{
	setKeyboardEnabled(false);
}



XUiLayout* XUiLayout::createWith(Layout* _layout)
{
	XUiLayout* _res = new XUiLayout();
	if (_res) {
		//_res->setContentSize(_layout->getContentSize());

		_res->copyProperties(_layout);

		_res->copyClonedWidgetChildren(_layout);
		_res->autorelease();

		Node* _p = _layout->getParent();
		if (_p) _p->addChild(_res, _layout->getZOrder());

		_layout->removeFromParent();

		_res->setKeyboardEnabled(true);

		return _res;
	}

	return NULL;

}




void XUiLayout::setKeyboardEnabled(bool enabled)
{
	if (enabled != _keyboardEnabled)
	{
		_keyboardEnabled = enabled;

		_eventDispatcher->removeEventListener(_keyboardListener);
		_keyboardListener = nullptr;

		if (enabled)
		{
			auto listener = EventListenerKeyboard::create();
			listener->onKeyPressed = CC_CALLBACK_2(XUiLayout::onKeyPressed, this);
			listener->onKeyReleased = CC_CALLBACK_2(XUiLayout::onKeyReleased, this);

			_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
			_keyboardListener = listener;
		}
	}
}


void XUiLayout::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* unused_event)
{
	CC_UNUSED_PARAM(keyCode);
	CC_UNUSED_PARAM(unused_event);
}

void XUiLayout::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* unused_event)
{

	if (unused_event->isStopped()) return;

	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE) {

		if (m_defaultWidget && _clickEventListenerDefault) {
			//m_defaultWidget->releaseUpEvent();
			_clickEventListenerDefault(m_defaultWidget);
		}

		unused_event->stopPropagation();
		return;
	}


	//	CC_UNUSED_PARAM(unused_event);
#if CC_ENABLE_SCRIPT_BINDING
	if (kScriptTypeNone != _scriptType)
	{
		KeypadScriptData data(keyCode, this);
		ScriptEvent event(kKeypadEvent, &data);
		ScriptEngineManager::getInstance()->getScriptEngine()->sendEvent(&event);
	}
#endif




}

void XUiLayout::SetDefaultWidget(std::string _name, ccWidgetClickCallback _callback)
{
	m_defaultWidget = (Helper::seekWidgetByName(this, _name));
	_clickEventListenerDefault = _callback;
};


//////////////////////////////////////////////////////////////////////////
