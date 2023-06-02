#pragma once
#include "ga_result.h"
#include <stdint.h>

enum GaResult ga_audio_output_init(uint32_t sample_rate);
enum GaResult ga_audio_output_close(void);