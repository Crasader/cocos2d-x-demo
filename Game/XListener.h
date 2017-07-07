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

	//渲染层发送改变
	virtual void OnRenderDraw(int opc, void* buf, size_t sz, void* p1, void* p2) {};

};



#endif // XListener_h__
