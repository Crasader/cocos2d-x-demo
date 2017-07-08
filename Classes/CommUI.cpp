
#include "CommUI.h"


Vec2 theUICenter;

void XUIInit()
{
	cocos2d::Size sz = Director::getInstance()->getVisibleSize();
	theUICenter.x = sz.width / 2;
	theUICenter.y = sz.height / 2;
}
