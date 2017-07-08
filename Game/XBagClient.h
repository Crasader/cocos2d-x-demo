#ifndef XBagCLIENT_h__
#define XBagCLIENT_h__

#include "XBag.h"
#include "XBagIO.h"

class GxBagCellChangeItem
{
public:
	
};


class GxBagClient : public GxBag,public GxBagDataSourceINet
{
public:
	std::string char_guid; //角色唯一编号

public:

	GxBagClient();
	virtual ~GxBagClient();



};


#endif // XBagIO_h__
