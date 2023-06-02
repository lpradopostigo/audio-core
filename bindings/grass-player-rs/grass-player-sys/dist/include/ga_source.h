#pragma once
#include <stdint.h>
#include <wchar.h>

/**
 * @brief Represents a source of audio data.
 * @remark You can read the fields but you should not modify them.
 */
struct GaSource {
  const char* path;
  size_t size;
  const wchar_t* wpath;
};

struct GaSource* gp_new_source(const char* path);
void ga_free_source(struct GaSource* source);