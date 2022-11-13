#pragma once
#include <wchar.h>
#include <stdio.h>
#include "stdint.h"

#define WARN_IF(condition, message) do { if (condition) { printf("WARN: %s\n",message); } } while (0)

wchar_t* GA_Utf8ToWstring(const char* utf8);
uint16_t GA_ResolveIndex(int16_t index, uint16_t size);
const char* GA_Concat(const char* str1, const char* str2);

