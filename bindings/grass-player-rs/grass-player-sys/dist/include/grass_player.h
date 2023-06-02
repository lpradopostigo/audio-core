#pragma once
#include <stdint.h>

enum GpResult {
  GP_RESULT_ERROR = 0,
  GP_RESULT_OK = 1,
};

enum GpPlaybackState {
  GP_PLAYBACK_STATE_STOPPED = 0,
  GP_PLAYBACK_STATE_PLAYING = 1,
  GP_PLAYBACK_STATE_PAUSED = 2,
};

enum GpSampleRate {
  GP_SAMPLE_RATE_44100 = 44100,
  GP_SAMPLE_RATE_48000 = 48000,
};

enum GpResult gp_init(enum GpSampleRate sample_rate);
enum GpResult gp_close(void);

enum GpResult gp_set_sources(const char** sources, size_t sources_size);
enum GpPlaybackState gp_get_playback_state(void);
size_t gp_get_sources_size(void);
size_t gp_get_source_index(void);
const char* gp_get_source_path(void);
double gp_get_source_position(void);
double gp_get_source_duration(void);
float gp_get_volume(void);
void gp_set_volume(float volume);

void gp_play(void);
void gp_stop(void);
void gp_pause(void);
void gp_seek(double seconds);
void gp_skip_to(size_t source_index);
