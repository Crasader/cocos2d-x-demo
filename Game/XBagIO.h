#ifndef XBagIO_h__
#define XBagIO_h__

#include "XBag.h"


/**
从网络中取得
*/
class GxBagDataSourceINet : public GxBagDataSource
{
public:
	virtual void Reload(GxBag* target);

	/**
	收到背包信息反馈后处理
	*/
	void OnReloadResponse(const char* buf,size_t sz);

};



#endif // XBagIO_h__
