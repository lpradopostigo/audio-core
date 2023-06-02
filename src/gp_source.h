#pragma once
#include <stdint.h>
#include <wchar.h>

struct GpSource {
  const char* path;
  size_t size;
  const wchar_t* wpath;
};

struct GpSource* gp_new_source(const char* path);
void gp_free_source(struct GpSource* source);
