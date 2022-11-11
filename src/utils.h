#pragma once
#include <wchar.h>
#include <stdio.h>

#define WARN_IF(condition, message) do { if (condition) { printf("WARN: %s\n",message); } } while (0)

wchar_t* GA_Utf8ToWstring(const char* utf8);
int GA_ResolveIndex(int index, int size);
const char* GA_Concat(const char* str1, const char* str2);

