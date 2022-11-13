#include <windows.h>
#include "utils.h"

wchar_t* GA_Utf8ToWstring(const char* utf8) {
	const size_t utf8_length = strlen(utf8);
	const size_t wstr_length = MultiByteToWideChar(CP_UTF8, 0, utf8, (int)utf8_length, NULL, 0);

	wchar_t* wstr = (wchar_t*)malloc(sizeof(wchar_t) * (wstr_length + 1));

	MultiByteToWideChar(CP_UTF8, 0, utf8, (int)utf8_length, wstr, (int)wstr_length);
	wstr[wstr_length] = L'\0';

	return wstr;

}

uint16_t GA_ResolveIndex(int16_t index, uint16_t size) {
	if (index < 0) {
		return size - 1;
	}
	else if (index >= size) {
		return 0;
	}
	else {
		return index;
	}
}

const char* GA_Concat(const char* str1, const char* str2) {
	const size_t str1_length = strlen(str1);
	const size_t str2_length = strlen(str2);
	const size_t result_length = str1_length + str2_length;
	char* result = (char*)malloc(result_length + 1);
	memcpy(result, str1, str1_length);
	memcpy(result + str1_length, str2, str2_length);
	result[result_length] = '\0';
	return result;
}

