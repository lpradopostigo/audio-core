#pragma once
#include <stdint.h>
#include <wchar.h>

struct GaSource {
  const char* path;
  size_t size;
  const wchar_t* wpath;
};

struct GaSource* ga_new_source(const char* path);
void ga_free_source(struct GaSource* source);