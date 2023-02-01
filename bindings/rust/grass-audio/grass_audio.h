#pragma once
#include <stdint.h>

enum GA_Result {
  GA_RESULT_OK = 0,
  GA_RESULT_ERROR = 1
};

enum GA_PlaybackState {
  GA_PLAYBACK_STATE_STOPPED = 0,
  GA_PLAYBACK_STATE_PLAYING = 1,
  GA_PLAYBACK_STATE_PAUSED = 2
};

enum GA_Result GA_Init(uint32_t sample_rate, const char* plugin_path);
enum GA_Result GA_Terminate(void);
void GA_SetPlaylist(char const* const* playlist, uint16_t playlist_size);
void GA_Play(void);
void GA_Pause(void);
void GA_Stop(void);
void GA_Previous(void);
void GA_Next(void);
void GA_SetVolume(float volume);
float GA_GetVolume(void);
void GA_Seek(double position);
void GA_SkipToTrack(int16_t index);
uint16_t GA_GetCurrentTrackIndex(void);
uint16_t GA_GetPlaylistSize(void);
enum GA_PlaybackState GA_GetPlaybackState(void);
double GA_GetTrackPosition(void);
double GA_GetTrackLength(void);
