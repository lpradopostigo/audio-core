#pragma once
#include <string>

namespace util {
#ifdef WIN32
std::wstring utf8_to_wstring(const std::string& str);
#endif
}



