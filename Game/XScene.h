#ifndef XScene_h__
#define XScene_h__

#include "XObject.h"
#include "XPlayer.h"

namespace xg
{

	class  CxScene : public  CxObject
	{
	private:
		static CxScene* _instance;

	protected:
		CxScene();

	public:
		virtual ~CxScene();


		static CxScene* Instance();

		GxPlayer* PlayerNew();
		void PlayerDelete(GxPlayer* ply);


	};


};




#endif // XScene_h__
