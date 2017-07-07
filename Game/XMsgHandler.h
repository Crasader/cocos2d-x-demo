#ifndef XMsgHandler_h__
#define XMsgHandler_h__

#include <stdint.h>

/*
网络来的消息处理方法
*/

typedef int(*XMSG_HANDLER)(uint16_t,const char*,size_t);


//有游戏单元进入所在的场景
int xmsg_handler_obj_add(uint16_t, const char*, size_t);


//有游戏单元离开所在的场景
int xmsg_handler_obj_del(uint16_t, const char*, size_t);




#endif // XMsgHandler_h__
