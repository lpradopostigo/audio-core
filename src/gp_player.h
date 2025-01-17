#pragma once
#include <stdint.h>
#include "grass_player.h"
#include "gp_source_list.h"

struct GpPlayer {
  struct GpSourceList* sources;
  size_t source_index;
  uint32_t stream_handle;
  uint32_t mixer_stream_handle;
};

