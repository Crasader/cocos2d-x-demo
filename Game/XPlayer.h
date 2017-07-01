
#ifndef XPlayer_h__
#define XPlayer_h__

#include "XActor.h"

namespace xg
{
	/**
	不同玩家控制的演员
	*/
	class CxPlayer : public CxActor
	{
	private:
		static CxPlayer* _myself;

	public:
		string m_acct_uuid; //账号 唯一的 在游戏中
		string m_session;  //会话令牌

		string m_password; //密码

//放在属性中
//		uint64_t m_gold;//绑金
//		uint64_t m_coin;//游戏币

	public:

		/*
		玩家自己
		*/
		static CxPlayer* Instance();


	};


};




#endif 
