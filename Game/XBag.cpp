

#include "XBag.h"

std::unordered_map<XBC_INT, XBC_INT> theItemRulerCombine;
std::unordered_map<XBC_INT, XBC_INT> theItemRulerSort;


void GxBag::SetDataSource(GxBagDataSource* _source)
{
	m_pSource = _source;
}

void GxBag::SetDataStorage(GxBagDataStorage* _storage)
{
	m_pStorage = _storage;
}

void GxBag::Reload()
{
	if (m_pSource) {
		m_pSource->Reload(this);
	}
}
