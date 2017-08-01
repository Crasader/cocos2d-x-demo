
#ifndef XLanguage_h__
#define XLanguage_h__



#include <string.h>
#include <string>
#include <sstream>
#include <vector>

namespace XLanguage
{
	const char* GetString(std::string idn);
	int Parse(const char* txt, size_t sz);
	int Clear();

	int GetCurrentCode();
	int SetCurrentCode(int _code);

};


#endif // XStrHelper_h__