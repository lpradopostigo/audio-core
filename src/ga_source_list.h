#pragma once
#include "ga_source.h"

struct GaSourceList {
  struct GaSource** list;
  size_t size;
};

struct GaSourceList* ga_new_source_list(const char** paths, size_t size);
void ga_free_source_list(struct GaSourceList* source_list);

