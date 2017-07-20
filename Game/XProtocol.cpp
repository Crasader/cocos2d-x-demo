
#include "XProtocol.h"
#include "gayola/ByteBuffer.h"
#include "gayola/CxTcpClient.h"
#include "opcodes.h"

namespace XPTO_GAME
{
	using namespace xs;

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



	void s_char_enum(const char* _buf, size_t _sz)
	{
		//╫ги╚ап╠М
	}



	void c_char_use(std::string _name)
	{
		if (_name.empty()) return;

		ByteBuffer bbf;
		bbf << (uint16_t)XCMSG_CHAR_USE;
		bbf << _name;
		CxTcpClient::shared()->SendData(bbf.contents(), bbf.size());
	}



};
