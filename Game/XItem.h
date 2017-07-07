#ifndef XItem_h__
#define XItem_h__

#include <stdint.h>
#include <unordered_map>

typedef uint16_t XTI_INT;

class GxItem
{
public:
	char order; //排序权值
	XTI_INT  itemId;
	XTI_INT  combine_num;	//一个背包格子中最大能放多少
};




#endif // XItem_h__
