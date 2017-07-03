
#include "XScene.h"

namespace xg
{

	CxScene* CxScene::_instance=NULL;

	CxScene::CxScene()
	{

	}

	CxScene::~CxScene()
	{

	}

	CxScene* CxScene::Instance()
	{
		if (_instance == NULL) _instance = new CxScene();
		return _instance;
	}




	GxPlayer* CxScene::PlayerNew()
	{
		GxPlayer* ply = new GxPlayer();
		m_childs.insert(ply);
		ply->m_parent = this;
		return ply;
	}



	void CxScene::PlayerDelete(GxPlayer* ply)
	{
		CxObject::ChildDelete(ply);
	}

};

