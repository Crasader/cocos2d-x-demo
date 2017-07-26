
#ifndef XPlayer_h__
#define XPlayer_h__

#include "XActor.h"

namespace xg
{
	/**
	不同玩家控制的演员
	*/
	class GxPlayer : public CxActor
	{


	public:
		string m_acct_uuid; //账号 唯一的 在游戏中

		uint64_t char_guid;

//放在属性中
//		uint64_t m_gold;//绑金
//		uint64_t m_coin;//游戏币

		string m_body_part;
	public:




	};


};




#endif 
