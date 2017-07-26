#ifndef XScene_h__
#define XScene_h__

#include "XObject.h"
#include "XPlayer.h"

namespace xg
{

	class  GxScene : public  GxObject
	{
	private:
		static GxScene* _instance;

	protected:
		

	public:
		GxScene();
		virtual ~GxScene();


		//static GxScene* Instance();

		GxPlayer* PlayerNew();
		void PlayerDelete(GxPlayer* ply);

		/**
		网络来的消息
		*/
		void OnMessage(const char* buf,size_t sz);

		GxPlayer* FindPlayerByName(std::string _name);
	};


};




#endif // XScene_h__
