#ifndef GxError_h__
#define GxError_h__


#include <string>




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

enum ErrCodesST
{
	XEC_UNKNOW,
	XEC_OK,											//没有错误

	XEC_C_NET_DISCONN,								//客户端网络断开

	XEC_NET_PROTOCOL,								//通信协议号太低
	XEC_AUTH_ERROR,									//账号或者密码错误
	XEC_AUTH_PASS,									//登录验证通过 此账号没有登录过此游戏 客户端可以开始选择服务器

	XEC_CHAR_NAME_EXIST,							//角色名称已经存在
	XEC_CHAR_NAME_INVALID,							//角色名非法校验在客户端做
	XEC_CHAR_NUM_OVER,								//角色数量达到限制最大

	XEC_TASK_OVERLOAD,								//任务数量超限
	XEC_TASK_NOT_FOUND,								//任务没发现
	XEC_TASK_HAS_EXIST,								//任务已经存在
	XEC_TASK_CANNOT_CANCEL,							//不可取消的任务
	XEC_TASK_PLAYER_LV_OVERLOAD,					//玩家的等级不再限制范围内
	XEC_TASK_PLAYER_CAMP_NOT_MATCH,					//玩家的阵营不匹配
	XEC_TASK_NOT_FINISH,							//任务没有完成
	XEC_TASK_NOT_EXIST_FOLLOW,						//没有后续任务
	XEC_TASK_PRE_NOT_FINISH,						//前置任务未完成
	XEC_TASK_PLAYER_CAREER_NOT_MATCH,				//玩家的阵营不匹配
	XEC_TASK_NOT_IN_ACCEPT_TIME_RANGE,				//不再接任务的时间范围内
};


struct gx_error_s
{
	int code;
	char text[255];

	gx_error_s& operator=(const struct gx_error_s& _R)
	{
		code = _R.code;
		memcpy(text, _R.text, 255);
		return *this;
	}

	gx_error_s()
	{
		code = 0;
		memset(text, 0, 255);
	}

	gx_error_s(int _c,const char* _t)
	{
		code = _c;
		if(_t) snprintf(text, 255, "%s", _t);
	}


	gx_error_s(const struct gx_error_s& _R)
	{
		code = _R.code;
		memcpy(text, _R.text, 255);
	}

};

typedef struct gx_error_s  gx_error_t;

#endif // GxError_h__
