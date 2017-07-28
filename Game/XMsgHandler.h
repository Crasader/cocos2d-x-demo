#ifndef XMsgHandler_h__
#define XMsgHandler_h__

#include <stdint.h>

#include "client_codes.h"

/*
消息处理回调
*/
typedef int(*XMSG_HANDLER)(uint16_t,const char*,size_t);


//有游戏单元进入所在的场景
int xmsg_handler_obj_add(uint16_t, const char*, size_t);


//有游戏单元离开所在的场景
int xmsg_handler_obj_del(uint16_t, const char*, size_t);


class GxMsgHandler
{
public:
	virtual ~GxMsgHandler() {};
	virtual int OnGxMessage(const char* buf,size_t sz,void* arg)=0;
};

struct gxmsginfo_s
{
	GxMsgHandler* m_handler;
	int m_order;
};

typedef struct gxmsginfo_s gxmsginfo_t;


/**
网络消息处理回调
@param buf 网络传过来的数据
@param sz 网络传来数据大小
@param arg 底层传递来的数据指针
@param userdata 是上层来的时传递的参数
*/
typedef int(*XNET_MSG_HANDLER)(const char* buf, size_t sz,void* arg,void* userdata);


struct gx_net_msg_handler_s
{
	XNET_MSG_HANDLER handler;
	void* userdata;
	int order;
};

typedef struct gx_net_msg_handler_s gx_net_msg_handler_t;


#endif // XMsgHandler_h__
