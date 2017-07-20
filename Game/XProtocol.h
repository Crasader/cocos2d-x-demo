#ifndef XProtocol_h__
#define XProtocol_h__

#include <string>


namespace XPTO_GAME
{

	void c_char_enum();
	void c_char_create(std::string _name);

	void s_char_enum(const char* _buf,size_t _sz);
	
	void c_char_rename(std::string _name);
	void s_char_rename(const char* _buf, size_t _sz);

	void c_char_use(std::string _name);

	void c_scene_enter(std::string _name);
	void c_scene_exit();

	//Àë¿ªÓÎÏ·
	void c_game_exit();

};



#endif // XProtocol_h__
