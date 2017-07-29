
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

	//GxScene* GxScene::Instance()
	//{
	//	if (_instance == NULL) _instance = new GxScene();
	//	return _instance;
	//}




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

	GxPlayer* GxScene::FindPlayerByName(std::string _name)
	{
		for (auto it : m_childs) {
			GxObject* obj = it;
			GxPlayer* ply = dynamic_cast<GxPlayer*>(obj);
			if (ply && ply->m_name.compare(_name) == 0) {
				return ply;
			}
		}
		return NULL;
	}

	void GxScene::clear()
	{
		ChildRemoveAll();
	}

};

