#ifndef XMsgHandler_h__
#define XMsgHandler_h__

#include <stdint.h>

#include "client_codes.h"

/*
网络来的消息处理方法
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


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
typedef int(*XNET_MSG_HANDLER)(const char* buf, size_t sz,void* arg,void* userdata);

struct gx_net_msg_handler_s
{
	XNET_MSG_HANDLER handler;
	void* userdata;
	int order;
};

typedef struct gx_net_msg_handler_s gx_net_msg_handler_t;


#endif // XMsgHandler_h__
