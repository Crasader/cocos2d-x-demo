#ifndef XProtocol_h__
#define XProtocol_h__

#include <string>
#include <map>
#include <stdint.h>

namespace XPTO_GAME
{

	typedef int(*DORESPONSE)(const char* buf, size_t sz, void* arg, void* userdata);

	extern std::map<uint16_t, DORESPONSE> theCntDoResponse;

	void Init();

	void c_char_enum();
	void c_char_create(std::string _name);
	void c_char_rename(std::string _name);
	void c_char_use(std::string _name);
	void c_scene_enter(std::string _name);
	void c_scene_exit();
	void c_game_exit();

	int NetMsgHandler(const char* buf, size_t sz, void* arg, void* userdata);

};



#endif // XProtocol_h__
