#ifndef XListener_h__
#define XListener_h__

class GxListener
{
public:
	virtual ~GxListener() {};

	//显示文本发生改变
	virtual void OnChangeDisplyString(const char* txt) {};

	//发生错误信息
	virtual void OnError(int ecode, const char* txt) {};

};



#endif // XListener_h__
