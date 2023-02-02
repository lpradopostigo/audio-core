#pragma once
#include <wchar.h>
#include <stdio.h>
#include "stdint.h"

wchar_t* GA_Utf8ToUtf16(const char* utf8);
char* GA_Utf16ToUtf8(const wchar_t* utf16);
uint16_t GA_ResolveIndex(int16_t index, uint16_t size);
const char* GA_Concat(const char* str1, const char* str2);

