
#include "XGame.h"
#include "gayola/ByteBuffer.h"

X_IMPL_SINSTANCE(GxApplication)


int GameDirectorMsg(char* buf, size_t sz, void* arg)
{
	return GxApplication::Instance()->OnMessage(buf, sz, arg);
}


GxApplication::GxApplication()
{

}

GxApplication::~GxApplication()
{

}

GxPlayer& GxApplication::MySelf()
{
	return m_mySelf;
}

GxScene& GxApplication::MyScene()
{
	return m_myScene;
}

int GxApplication::OnMessage(char* buf, size_t sz, void* arg)
{

	ByteReader brr(buf, sz);
	uint16_t msgId = brr.read<uint16_t>();
	if (msgId == XSMSG_AUTH_URL)
	{
		brr >> m_strDisplay;
		DisplayStringSet(m_strDisplay.c_str());
		return 1;
	}

	return 0;
}

void GxApplication::DisplayStringSet(const char* txt)
{
	if (txt == NULL) return;
	m_strDisplay = string(txt);
	for (auto it : m_gxListener) {
		it->OnChangeDisplyString(m_strDisplay.c_str());
	}
}

const char* GxApplication::DisplayStringGet()
{
	return m_strDisplay.c_str();
}

void GxApplication::LastErrorSet(int n, const char* txt)
{
	m_iErrorCode = n;
	if(txt) m_strError = string(txt);
	for (auto it : m_gxListener) {
		it->OnError(n, m_strError.c_str());
	}

}

void GxApplication::GxListenerAdd(GxListener* arg)
{
	m_gxListener.insert(arg);
}

void GxApplication::GxListenerDel(GxListener* arg)
{
	m_gxListener.erase(arg);
}
