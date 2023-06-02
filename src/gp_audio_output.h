#pragma once
#include "grass_player.h"

struct Plugin {
  const char* path;
  uint32_t handle;
};

enum GpResult gp_audio_output_init(enum GpSampleRate sample_rate);
enum GpResult gp_audio_output_close(void);
