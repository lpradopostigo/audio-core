#pragma once
#include "gp_source.h"

struct GpSourceList {
  struct GpSource** list;
  size_t size;
};

struct GpSourceList* gp_new_source_list(const char** paths, size_t size);
void gp_free_source_list(struct GpSourceList* source_list);

