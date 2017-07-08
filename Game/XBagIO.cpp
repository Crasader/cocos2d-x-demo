
#include "XBagIO.h"
#include "XGame.h"

void GxBagDataSourceINet::Reload(GxBag* target)
{
	//向服务器发送请求消息
	//本地生成一个请求队列


	//GxApplication::Instance()->SendToServer();

}

void GxBagDataSourceINet::OnReloadResponse(const char* buf, size_t sz)
{

}

