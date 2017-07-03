#include "XPlayer.h"


namespace xg
{





	GxPlayer* GxPlayer::_myself=NULL;

	GxPlayer* GxPlayer::Instance()
	{
		if (_myself == NULL) _myself = new GxPlayer();
		return _myself;
	}



};

