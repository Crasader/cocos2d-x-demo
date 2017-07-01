#include "XPlayer.h"


namespace xg
{





	CxPlayer* CxPlayer::_myself=NULL;

	CxPlayer* CxPlayer::Instance()
	{
		if (_myself == NULL) _myself = new CxPlayer();
		return _myself;
	}



};

