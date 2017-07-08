#ifndef XBag_h__
#define XBag_h__


#include <vector>
#include <list>
#include <stdint.h>
#include <unordered_map>

#include "XItem.h"

typedef int16_t XBC_INT;

extern std::unordered_map<XBC_INT, XBC_INT> theItemRulerCombine;//物品道具合并数量 guid,num
extern std::unordered_map<XBC_INT, XBC_INT> theItemRulerSort;	//物品道具排序顺序 guid,order
extern std::unordered_map<XBC_INT, XBC_INT> theItemRulerType;	//物品道具分类 guid,typeid
extern std::unordered_map<XBC_INT, XBC_INT> theItemRulerPage;	//物品道具分在哪个页面上 guid,pageid

class GxBagCell
{
public:
	XBC_INT pos; //位置
	int item_id_my;	 //在自己上的编号 初次获得时间
	XBC_INT item_id;	 //物品道具编号
	XBC_INT item_num;	 //数量

	char pageId;	//分页

//private:
//	char type;  //物品道具分类
//	char order; //道具排序权值

};

class GxBagPage
{
public:
	char pageId;  //分页编号
	XBC_INT total_open_cell;	//当前总共开启了多少格子
	XBC_INT max_cell;	//格子最大值

public:
	char type;
	char order;
	std::string name;	//页面名字

public:
	

protected:
	std::vector<XBC_INT> reuse_pos;

public:
	void sort();	//整理
	void sort_r(); //反向整理

	//拆分
	void Split(XBC_INT _pos, XBC_INT _num);

	/**
	增加物品
	根据物品配表进行合并
	*/
	GxBagCell* AddItem(XBC_INT _iid, XBC_INT _num);

	//获取当前可用的位置 没有可用位置返回 -1
	XBC_INT GetAvailablePos();
	XBC_INT IsAvailablePos(XBC_INT _pos);

};

class GxBagDataSource;
class GxBagDataStorage;
class GxBagListener;

class GxBag
{
public:
	
	uint64_t m_gold;
	uint64_t m_coin;
	uint64_t m_exp; //经验

	std::vector<GxBag> pages;
	
	std::vector<GxBagCell> cells;

protected:
	GxBagDataSource* m_pSource;
	GxBagDataStorage* m_pStorage;

public:
	void SetDataSource(GxBagDataSource* _source);
	void SetDataStorage(GxBagDataStorage* _storage);

	void Reload();
	void Save();	//保存所有 上下线
	void SaveBase(); //保存基本信息 5分钟一次
	
	/**
	增加物品
	根据物品配表进行合并
	*/
	GxBagCell* AddItem(XBC_INT _iid, XBC_INT _num);

	//用掉物品
	bool RemoveItem(GxBagCell _cell, XBC_INT _item_num);

	//移动物品到新的位置上
	bool SetPos(GxBagCell _cell, XBC_INT _pos);

};

class GxBagDataSource
{
public:
	virtual void Reload(GxBag* target)=0;

};

class GxBagDataStorage
{

};

class GxBagListener
{
public:
	/**
	如果不返回 0 值 后面将不真正执行
	*/
	virtual int OnBagCellChangeBefore(GxBag* sender, GxBagCell* _old, GxBagCell* _new) {};
	virtual void OnBagCellChangeAfter(GxBag* sender, GxBagCell* _old, GxBagCell* _new) {};
};

/**
用在游戏服务器端
*/
class GxBagServer : public GxBag
{

};



#endif // XBag_h__
