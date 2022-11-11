#pragma once
#include "bass.h"

#define GA_NO_HANDLER 0
#define GA_OK 0
#define GA_ERROR 1

#define GA_PLAYBACK_STATE_STOPPED BASS_ACTIVE_STOPPED
#define GA_PLAYBACK_STATE_PLAYING BASS_ACTIVE_PLAYING
#define GA_PLAYBACK_STATE_PAUSED BASS_ACTIVE_PAUSED
#define GA_PLAYBACK_STATE_STALLED BASS_ACTIVE_STALLED
#define GA_PLAYBACK_STATE_PAUSED_DEVICE BASS_ACTIVE_PAUSED_DEVICE

struct GA_Player {
  HSTREAM mixer_stream;
  HSTREAM current_stream;
  HSYNC track_end_sync_handler;
  int current_track_index;
  wchar_t** playlist;
  int playlist_size;
};

int GA_Init(DWORD sample_rate, const char* plugin_path);
int GA_Terminate();
struct GA_Player* GA_CreatePlayer(DWORD sample_rate);
int GA_DestroyPlayer(struct GA_Player* player);
void GA_SetPlaylist(struct GA_Player* player, char const* const* playlist, int playlist_size);
void GA_Play(struct GA_Player* player);
void GA_Pause(struct GA_Player* player);
void GA_Stop(struct GA_Player* player);
void GA_Previous(struct GA_Player* player);
void GA_Next(struct GA_Player* player);
void GA_SetVolume(struct GA_Player* player, float volume);
float GA_GetVolume(struct GA_Player* player);
void GA_Seek(struct GA_Player* player, double position);
void GA_SkipToTrack(struct GA_Player* player, int index);
int GA_GetCurrentTrackIndex(struct GA_Player* player);
int GA_GetPlaylistSize(struct GA_Player* player);
DWORD GA_GetPlaybackState(struct GA_Player* player);
double GA_GetTrackPosition(struct GA_Player* player);
double GA_GetTrackLength(struct GA_Player* player);
