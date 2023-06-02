#pragma once
#include <stdint.h>
#include "ga_source_list.h"
#include "ga_result.h"

enum GaPlayerPlaybackState {
  GA_PLAYER_PLAYBACK_STATE_STOPPED,
  GA_PLAYER_PLAYBACK_STATE_PLAYING,
  GA_PLAYER_PLAYBACK_STATE_PAUSED,
};

/**
 * @brief Represents a player.
 * @remark You can read the fields but you should not modify them.
 * */
struct GaPlayer {
  struct GaSourceList* sources;
  size_t source_index;
  /** @internal */
  uint32_t stream_handle;
  /** @internal */
  uint32_t mixer_stream_handle;
};

struct GaPlayer* ga_new_player(uint32_t sample_rate);
void ga_free_player(struct GaPlayer* player);

enum GaResult ga_player_set_sources(struct GaPlayer* player, struct GaSourceList* sources);
enum GaPlayerPlaybackState ga_player_get_playback_state(struct GaPlayer* player);
double ga_player_get_source_position(struct GaPlayer* player);
double ga_player_get_source_length(struct GaPlayer* player);
float ga_player_get_volume(struct GaPlayer* player);
void ga_player_set_volume(struct GaPlayer* player, float volume);
const char* ga_player_get_source_path(struct GaPlayer* player);

void ga_player_play(struct GaPlayer* player);
void ga_player_stop(struct GaPlayer* player);
void ga_player_pause(struct GaPlayer* player);
void ga_player_seek(struct GaPlayer* player, double seconds);
void ga_player_skip_to(struct GaPlayer* player, size_t index);
