#pragma once
#include <stdint.h>

enum GAResult {
  GA_RESULT_OK = 0,
  GA_RESULT_ERROR = 1
};

enum GAPlaybackState {
  GA_PLAYBACK_STATE_STOPPED = 0,
  GA_PLAYBACK_STATE_PLAYING = 1,
  GA_PLAYBACK_STATE_PAUSED = 2
};

enum GAResult ga_init(uint32_t sample_rate);
enum GAResult ga_terminate(void);
void ga_set_playlist(char const* const* playlist, uint16_t playlist_size);
void ga_play(void);
void ga_pause(void);
void ga_stop(void);
void ga_previous(void);
void ga_next(void);
void ga_set_volume(float volume);
float ga_get_volume(void);
void ga_seek(double position);
void ga_skip_to_track(int16_t index);
uint16_t ga_get_current_track_index(void);
const char* ga_get_current_track_path(void);
uint16_t ga_get_playlist_size(void);
enum GAPlaybackState ga_get_playback_state(void);
double ga_get_track_position(void);
double ga_get_track_length(void);
