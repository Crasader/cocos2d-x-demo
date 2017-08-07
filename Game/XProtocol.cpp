
#include "XProtocol.h"
#include "gayola/ByteBuffer.h"
#include "gayola/ByteReader.h"
#include "gayola/CxTcpClient.h"
#include "opcodes.h"
#include "XGame.h"
#include "GxError.h"

#include <map>
#include <assert.h>

namespace XPTO_GAME
{
	using namespace xs;
	using namespace std;

	static SENDTOCLIENT appSendToClient = NULL;

	//typedef int(*DORESPONSE)(const char* buf, size_t sz, void* arg, void* userdata);

	static std::map<uint16_t, DORESPONSE> theCntDoResponse;

	void c_char_enum()
	{
		ByteBuffer bbf;
		bbf << (uint16_t)XCMSG_CHAR_ENUM;
		CxTcpClient::shared()->SendData(bbf.contents(), bbf.size());
	}



	void c_char_create(std::string _name)
	{
		if (_name.empty()) _name = "<random>";

		ByteBuffer bbf;
		bbf << (uint16_t)XCMSG_CHAR_CREATE;
		bbf << _name;
		CxTcpClient::shared()->SendData(bbf.contents(), bbf.size());
	}

	void c_char_rename(std::string _name0,std::string _name1)
	{
		if (_name0.compare(_name1) == 0) return;

		ByteBuffer bbf;
		bbf << (uint16_t)XCMSG_CHAR_RENAME2;
		bbf << _name0;
		bbf << _name1;
		CxTcpClient::shared()->SendData(bbf.contents(), bbf.size());

	}

	void c_char_delete(std::string _name)
	{
		if (_name.empty()) return;

		ByteBuffer bbf;
		bbf << (uint16_t)XCMSG_CHAR_DELETE;
		bbf << _name;
		CxTcpClient::shared()->SendData(bbf.contents(), bbf.size());
	}

	void c_char_use(std::string _name)
	{
		if (_name.empty()) return;

		GxApplication::Instance()->m_mySelf.m_name = _name;
		GxScene& scn = GxApplication::Instance()->MyScene();
		GxPlayer* ply = scn.FindPlayerByName(_name);
		if (ply) {
			GxApplication::Instance()->m_mySelf.m_acct_uuid = ply->m_acct_uuid;
			//FIXME 
		}

		ByteBuffer bbf;
		bbf << (uint16_t)XCMSG_CHAR_USE;
		bbf << _name;
		CxTcpClient::shared()->SendData(bbf.contents(), bbf.size());
	}


	void c_myself_info()
	{
		//��ȡ�Լ�����Ϣ 
		ByteBuffer bbf;
		bbf << (uint16_t)XCMSG_MY_INFO;
		CxTcpClient::shared()->SendData(bbf.contents(), bbf.size());
	}

	int s_char_enum(const char* buf, size_t sz, void* arg, void* userdata)
	{
		GxApplication* app = (GxApplication*)userdata;
		GxScene& scn = app->MyScene();
		scn.ChildRemoveAll();

		ByteReader brr(buf+2, sz-2);
		char c = brr.read<char>();
		for (char i = 0; i < c; i++)
		{

			//
			GxPlayer* ply = scn.PlayerNew();
			ply->char_guid = brr.read<uint64_t>();
			ply->m_level= brr.read<uint16_t>();

			int8_t i8= brr.read<int8_t>();
			ply->AttribSet("sex", XOBJ_ATTR_TYPE::OAT_I8, (const char*)&i8, 1);

			i8 = brr.read<int8_t>();
			ply->AttribSet("career", XOBJ_ATTR_TYPE::OAT_I8, (const char*)&i8, 1);

			uint32_t _u32= brr.read<uint32_t>();
			ply->AttribSet("md_body", XOBJ_ATTR_TYPE::OAT_U32, (const char*)&_u32, 4);

			_u32 = brr.read<uint32_t>();
			ply->AttribSet("md_face", XOBJ_ATTR_TYPE::OAT_U32, (const char*)&_u32, 4);

			_u32 = brr.read<uint32_t>();
			ply->AttribSet("md_hair", XOBJ_ATTR_TYPE::OAT_U32, (const char*)&_u32, 4);

			brr >> ply->m_name;
			brr >> ply->m_body_part;

		}
		return 0;
	}


	void c_world_ready()
	{
		ByteBuffer bbf;
		bbf << (uint16_t)XCMSG_WORLD_READY;
		CxTcpClient::shared()->SendData(bbf.contents(), bbf.size());
	}

	int s_world_new(const char* buf, size_t sz, void* arg, void* userdata)
	{
		GxApplication* app = (GxApplication*)userdata;
		ByteReader brr(buf + 2, sz - 2);
		time_t _stime;
		brr >> _stime;

		time_t _now = time(NULL);
		app->m_iTimeDelta = difftime(_now, _stime);

		GxScene& scn = app->MyScene();
		brr >> scn.m_name;

		//��ȡ��ɫ�Լ���Ϣ ����
		c_myself_info();

		return 0;
	}


	int s_char_create(const char* buf, size_t sz, void* arg, void* userdata)
	{
		GxApplication* app = (GxApplication*)userdata;
		ByteReader brr(buf + 2, sz - 2);
		int err = brr.read<int>();
		if (err == XEC_OK) {
			c_char_enum();
		}
		return 0;
	}

	int s_char_rename(const char* buf, size_t sz, void* arg, void* userdata)
	{
		GxApplication* app = (GxApplication*)userdata;
		ByteReader brr(buf + 2, sz - 2);
		int err = brr.read<int>();
		string _name0,_name1;
		brr >> _name0;
		brr >> _name1;
		if (err == XEC_OK)
		{
		//	c_char_enum();

		}
		return 0;
	}

	int s_myself_info(const char* buf, size_t sz, void* arg, void* userdata)
	{
		GxApplication* app = (GxApplication*)userdata;
		ByteReader brr(buf + 2, sz - 2);
		GxPlayer* ply = app->Self();

		//ͷ�� vip lv coin gold
		int v = 0;
		brr >> v;
		ply->AttribSet("face", XOBJ_ATTR_TYPE::OAT_I32, (const char*)&v, 4);

		brr >> v;
		ply->AttribSet("vip", XOBJ_ATTR_TYPE::OAT_I32, (const char*)&v, 4);

		brr >> v;
		ply->AttribSet("level", XOBJ_ATTR_TYPE::OAT_I32, (const char*)&v, 4);

		uint64_t v64=0;
		brr >> v64;
		ply->AttribSet("coin", XOBJ_ATTR_TYPE::OAT_U64, (const char*)&v64, sizeof(uint64_t));

		brr >> v64;
		ply->AttribSet("gold", XOBJ_ATTR_TYPE::OAT_U64, (const char*)&v64, sizeof(uint64_t));


		return 0;
	}


	void Init()
	{
		theCntDoResponse[XSMSG_CHAR_ENUM] = s_char_enum;
		theCntDoResponse[XSMSG_WORLD_NEW] = s_world_new;
		theCntDoResponse[XSMSG_CHAR_CREATE] = s_char_create;
		theCntDoResponse[XSMSG_CHAR_RENAME] = s_char_rename;
		theCntDoResponse[XSMSG_MY_INFO] = s_myself_info;
		assert(appSendToClient);
	}


	void SetFunction(const char* _name, void* p)
	{
		assert(_name);
		assert(p);

		if (strcmp(_name, "appSendToClient") == 0) {
			appSendToClient = (SENDTOCLIENT)p;
			return;
		}

	}

	int NetMsgHandler(const char* buf, size_t sz, void* arg, void* userdata)
	{
		GxApplication* app = (GxApplication*)userdata;
		uint16_t* msgId = (uint16_t*)buf;

		auto it = theCntDoResponse.find(*msgId);
		if (it != theCntDoResponse.end()) {
			return (it->second)(buf, sz, arg, userdata);
		}

		return 0;
	}

};
