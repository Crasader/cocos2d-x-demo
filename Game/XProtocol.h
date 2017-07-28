#ifndef XProtocol_h__
#define XProtocol_h__

#include <string>
#include <map>
#include <stdint.h>

namespace XPTO_GAME
{

	typedef void(*SENDTOCLIENT)(const void* buf,size_t sz);

	typedef int(*DORESPONSE)(const char* buf, size_t sz, void* arg, void* userdata);

	extern std::map<uint16_t, DORESPONSE> theCntDoResponse;

	void Init();

	/**
	…Ë÷√∫Ø ˝÷∏’Î
	*/
	void SetFunction(const char* _name, void* p);

	void c_char_enum();
	void c_char_create(std::string _name);
	void c_char_rename(std::string _name);
	void c_char_use(std::string _name);

	void c_myself_info();

	void c_scene_enter(std::string _name);
	void c_scene_exit();
	void c_game_exit();

	void c_world_ready();

	int s_world_new(const char* buf, size_t sz, void* arg, void* userdata);
	int s_char_create(const char* buf, size_t sz, void* arg, void* userdata);


	int NetMsgHandler(const char* buf, size_t sz, void* arg, void* userdata);

};



#endif // XProtocol_h__
