#ifndef GxError_h__
#define GxError_h__


enum XLOGIN_ECODE
{
	XLE_PCODE = 1100,		//通信协议版本不兼容
	XLE_APP = 1200,			//应用名不支持
	XLE_USR_PWD = 1300,		//用户名或密码错误
	XLE_IN_GAME = 1500,		//账号已经在游戏中
	XLE_LOCK = 1400,		//账号被锁定了
	XLE_BLOCK = 1600,		//账号被阻止
	XLE_E_SID = 1301,		//会话令牌错误
	XLE_E_SID_EX = 1302,	//会话过期
};



#endif // GxError_h__
