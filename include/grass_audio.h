#pragma once
#include "bass.h"

#define GA_OK 0
#define GA_ERROR 1

#define GA_PLAYBACK_STATE_STOPPED BASS_ACTIVE_STOPPED
#define GA_PLAYBACK_STATE_PLAYING BASS_ACTIVE_PLAYING
#define GA_PLAYBACK_STATE_PAUSED BASS_ACTIVE_PAUSED
#define GA_PLAYBACK_STATE_STALLED BASS_ACTIVE_STALLED
#define GA_PLAYBACK_STATE_PAUSED_DEVICE BASS_ACTIVE_PAUSED_DEVICE

int GA_Init(DWORD sample_rate, const char* plugin_path);
int GA_Terminate();
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
DWORD GA_GetPlaybackState();
double GA_GetTrackPosition();
double GA_GetTrackLength();
