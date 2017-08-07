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

	XCMSG_SESSION=320,		//会话令牌验证
	XSMSG_SESSION,		//游戏网关针对令牌验证响应


#pragma mark ---- 基础 ----

	XMSG_NULL = 3000,

	XSMSG_ERROR,									//请求的操作返回错误
	XSMSG_ACTION_COMPLETED,							//客户端的命令成功执行

	XSMSG_SYSTEM_TIME,								//服务器的时间

	XSMSG_ACCOUNT_USE_OTHER,						//此账号其他客户端进行了登录此游戏操作



#pragma mark ---- 角色相关 ----

	XCMSG_CHAR_CREATE = 3100,							//创建角色
	XCMSG_CHAR_ENUM,								//获取角色列表
	XCMSG_CHAR_DELETE,								//删除指定角色
	XSMSG_CHAR_CREATE,								//成功创建 名称被占用 角色数量超限制
	XSMSG_CHAR_ENUM,								//角色列表	
	XSMSG_CHAR_DELETE,								//删除成功

	XCMSG_CHAR_USE,									//使用某个角色进入游戏

	XCMSG_CHAR_RENAME,								//改名字
	XSMSG_CHAR_RENAME,

	XCMSG_MY_INFO,									//获取自己的信息
	XSMSG_MY_INFO,
	XCMSG_CHAR_RENAME2,								//带着准备改名的角色旧名字



#pragma mark ---- 场景相关 ----

	XSMSG_WORLD_NEW= 3050,								//进入一个新的地图
	XCMSG_WORLD_READY,								//客户端世界准备就绪
	XCMSG_WORLD_ENTER,								//客户端角色请求进入一个新的地图 这里不用考虑是副本还是 只要消息号
	XCMSG_WORLD_EXIT,								//退出当前地图(只限在副本状态时)

	XCMSG_SCENE_LIST,								//场景列表
	XSMSG_SCENE_INFO,								//场景信息 名称等
	XSMSG_SCENE_INFO2,								//场景的详细信息
	XSMSG_SCENE_HEARTBEAT,							//场景心跳
	XMSG_SCENE_BROADCAST,							//场景广播

	XSMSG_OBJECT_ADD,								//增加一个游戏单元
	XSMSG_OBJECT_ADD_EX,							//增加一组游戏单元
	XSMSG_OBJECT_DEL,								//删除一个游戏单元
	XCMSG_OBJECT_INFO,								//查询游戏单元的外观信息
	XSMSG_OBJECT_INFO,


};


#endif // opcodes_h__
