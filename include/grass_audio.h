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
enum GA_Result GA_Terminate();
void GA_SetPlaylist(char const* const* playlist, int playlist_size);
void GA_Play();
void GA_Pause();
void GA_Stop();
void GA_Previous();
void GA_Next();
void GA_SetVolume(float volume);
float GA_GetVolume();
void GA_Seek(double position);
void GA_SkipToTrack(int index);
int GA_GetCurrentTrackIndex();
int GA_GetPlaylistSize();
enum GA_PlaybackState GA_GetPlaybackState();
double GA_GetTrackPosition();
double GA_GetTrackLength();
