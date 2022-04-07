#include "util.hpp"
#include <stringapiset.h>

namespace util {
std::wstring utf8_to_wstring(const std::string& str) {
	const auto str_length = static_cast<int>(str.length());
	const int wstr_length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str_length, nullptr, 0);
	std::wstring wstr(wstr_length, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str_length, &wstr[0], wstr_length);
	return wstr;
}
}