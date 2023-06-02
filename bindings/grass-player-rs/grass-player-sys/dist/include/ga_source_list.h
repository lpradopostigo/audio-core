#pragma once
#include "ga_result.h"
#include "ga_source.h"

/**
 * @brief Represents a list of audio sources.
 * @remark You can read the fields but you should not modify them.
 */
struct GaSourceList {
  struct GaSource** list;
  size_t size;
};

struct GaSourceList* gp_new_source_list(const char** paths, size_t size);
void ga_free_source_list(struct GaSourceList* source_list);

