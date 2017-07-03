
#include "XScene.h"

namespace xg
{

	GxScene* GxScene::_instance=NULL;

	GxScene::GxScene()
	{

	}

	GxScene::~GxScene()
	{

	}

	GxScene* GxScene::Instance()
	{
		if (_instance == NULL) _instance = new GxScene();
		return _instance;
	}




	GxPlayer* GxScene::PlayerNew()
	{
		GxPlayer* ply = new GxPlayer();
		m_childs.insert(ply);
		ply->m_parent = this;
		return ply;
	}



	void GxScene::PlayerDelete(GxPlayer* ply)
	{
		GxObject::ChildDelete(ply);
	}

};

