#ifndef opcodes_h__
#define opcodes_h__


enum opcodes
{
	XSMSG_BEGIN,
	XSMSG_AUTH_URL,
	XSMSG_LOGIN,
	XXMSG_TCP_EVENT,

	XCMSG_BAG_INFO,
	XCCOP_TCP_CONNECT,	//要求连接游戏服务器

	XCMSG_SESSION,		//会话令牌验证


};


#endif // opcodes_h__
