#pragma once
#include <stdint.h>
#include "grass_audio.h"
#include "ga_source_list.h"

struct GaPlayer {
  struct GaSourceList* sources;
  size_t source_index;
  uint32_t stream_handle;
  uint32_t mixer_stream_handle;

};

