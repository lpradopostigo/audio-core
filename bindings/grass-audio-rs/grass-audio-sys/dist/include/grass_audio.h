#pragma once
#include <stdint.h>

enum GaResult {
  GA_RESULT_OK = 0,
  GA_RESULT_ERROR = 1
};

enum GaPlayerPlaybackState {
  GA_PLAYER_PLAYBACK_STATE_STOPPED = 0,
  GA_PLAYER_PLAYBACK_STATE_PLAYING = 1,
  GA_PLAYER_PLAYBACK_STATE_PAUSED = 2,
};

enum GaResult ga_audio_output_init(uint32_t sample_rate);
enum GaResult ga_audio_output_close(void);

struct GaPlayer;

struct GaPlayer* ga_new_player(uint32_t sample_rate);
void ga_free_player(struct GaPlayer* player);

enum GaResult ga_player_set_sources(struct GaPlayer* player, const char** sources, size_t sources_size);
enum GaPlayerPlaybackState ga_player_get_playback_state(struct GaPlayer* player);
double ga_player_get_source_position(struct GaPlayer* player);
double ga_player_get_source_duration(struct GaPlayer* player);
float ga_player_get_volume(struct GaPlayer* player);
void ga_player_set_volume(struct GaPlayer* player, float volume);
const char* ga_player_get_source_path(struct GaPlayer* player);
size_t ga_player_get_source_index(struct GaPlayer* player);
size_t ga_player_get_sources_size(struct GaPlayer* player);

void ga_player_play(struct GaPlayer* player);
void ga_player_stop(struct GaPlayer* player);
void ga_player_pause(struct GaPlayer* player);
void ga_player_seek(struct GaPlayer* player, double seconds);
void ga_player_skip_to(struct GaPlayer* player, size_t index);
