#include "bass.h"
#include "bassmix.h"
#include "utils.h"
#include "grass_audio.h"

#define GA_NO_HANDLER 0

struct GA_Player {
  uint32_t sample_rate;
  uint32_t current_stream;
  uint32_t mixer_stream;
  wchar_t** playlist;
  uint16_t playlist_size;
  uint16_t current_track_index;
  uint32_t track_end_sync_handler;
};

void GA_SetTrackEndSync(void);
void GA_HandleTrackEndSync(void);
void GA_LoadStream(wchar_t* path);

static uint32_t ga_plugins[] = {GA_NO_HANDLER};
static struct GA_Player* ga_player = NULL;

enum GA_Result GA_Init(uint32_t sample_rate) {
	// don't init twice
	if (ga_player != NULL) return GA_RESULT_ERROR;

	// load BASS plugins
	if (ga_plugins[0] == GA_NO_HANDLER) {
		const char* bassflac_path = "./bassflac.dll";
		ga_plugins[0] = BASS_PluginLoad(bassflac_path, 0);
	}

	// init bass
	if (!BASS_Init(-1, sample_rate, 0, NULL, NULL)) return GA_RESULT_ERROR;

	// init player
	ga_player = (struct GA_Player*)malloc(sizeof(struct GA_Player));
	ga_player->sample_rate = sample_rate;
	ga_player->current_stream = GA_NO_HANDLER;
	ga_player->mixer_stream = GA_NO_HANDLER;
	ga_player->playlist = NULL;
	ga_player->playlist_size = 0;
	ga_player->current_track_index = 0;
	ga_player->track_end_sync_handler = GA_NO_HANDLER;

	return GA_RESULT_OK;
}

enum GA_Result GA_Terminate(void) {
	// don't terminate twice
	if (ga_player == NULL) return GA_RESULT_ERROR;

	// free bass plugins
	if (!BASS_PluginFree(ga_plugins[0])) return GA_RESULT_ERROR;
	ga_plugins[0] = GA_NO_HANDLER;

	// free bass
	if (!BASS_Free()) return GA_RESULT_ERROR;

	// free player
	for (uint16_t i = 0; i < ga_player->playlist_size; i++) {
		free((void*)ga_player->playlist[i]);
	}
	free((void*)ga_player->playlist);
	free(ga_player);
	ga_player = NULL;

	return GA_RESULT_OK;
}

void GA_SetPlaylist(char const* const* playlist, uint16_t playlist_size) {
	if (ga_player == NULL) return;

	// free old mixer stream and its resources
	if (ga_player->mixer_stream != GA_NO_HANDLER) {
		BASS_StreamFree(ga_player->mixer_stream);
		ga_player->current_stream = GA_NO_HANDLER;
		ga_player->track_end_sync_handler = GA_NO_HANDLER;
	}

	// replace mixer stream
	ga_player->mixer_stream = BASS_Mixer_StreamCreate(ga_player->sample_rate, 2, BASS_MIXER_END);

	// free old playlist
	if (ga_player->playlist != NULL) {
		for (uint16_t i = 0; i < ga_player->playlist_size; i++) {
			free((void*)ga_player->playlist[i]);
		}
		free((void*)ga_player->playlist);
	}

	// create new playlist
	wchar_t** new_playlist = (wchar_t**)malloc(sizeof(wchar_t*) * playlist_size);

	for (uint16_t i = 0; i < playlist_size; i++) {
		new_playlist[i] = GA_Utf8ToUtf16(playlist[i]);
	}

	// replace playlist
	ga_player->playlist = new_playlist;
	ga_player->playlist_size = playlist_size;
	ga_player->current_track_index = 0;
}

void GA_Play(void) {
	if (ga_player == NULL || ga_player->playlist == NULL) return;

	if (ga_player->current_stream == GA_NO_HANDLER) {
		GA_LoadStream(ga_player->playlist[ga_player->current_track_index]);
		GA_SetTrackEndSync();
	}

	BASS_ChannelPlay(ga_player->mixer_stream, FALSE);
}

void GA_SetTrackEndSync(void) {
	ga_player->track_end_sync_handler = BASS_ChannelSetSync(ga_player->mixer_stream,
			BASS_SYNC_END | BASS_SYNC_MIXTIME | BASS_SYNC_THREAD,
			0,
			(void (*)(HSYNC, DWORD, DWORD, void*))&GA_HandleTrackEndSync,
			NULL);
}

void GA_LoadStream(wchar_t* path) {
	ga_player->current_stream = BASS_StreamCreateFile(FALSE,
			path,
			0,
			0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_UNICODE);

	BASS_Mixer_StreamAddChannel(ga_player->mixer_stream, ga_player->current_stream,
			BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE);
	BASS_ChannelSetPosition(ga_player->mixer_stream, 0, BASS_POS_BYTE);
}

void GA_HandleTrackEndSync(void) {
	if (ga_player->current_track_index == ga_player->playlist_size - 1) {
		ga_player->current_track_index = 0;
		return;
	};

	ga_player->current_track_index++;
	GA_LoadStream(ga_player->playlist[ga_player->current_track_index]);
}

void GA_Stop(void) {
	if (ga_player == NULL) return;

	// free the mixer and its resources
	if (ga_player->mixer_stream != GA_NO_HANDLER) {
		BASS_StreamFree(ga_player->mixer_stream);
		ga_player->mixer_stream = GA_NO_HANDLER;
		ga_player->current_stream = GA_NO_HANDLER;
		ga_player->track_end_sync_handler = GA_NO_HANDLER;
	}

	ga_player->current_track_index = 0;
}

void GA_SkipToTrack(int16_t index) {
	if (ga_player == NULL || ga_player->playlist == NULL) return;
	BASS_ChannelRemoveSync(ga_player->mixer_stream, ga_player->track_end_sync_handler);
	ga_player->track_end_sync_handler = GA_NO_HANDLER;
	BASS_Mixer_ChannelRemove(ga_player->current_stream);
	ga_player->current_stream = GA_NO_HANDLER;
	ga_player->current_track_index = GA_ResolveIndex(index, ga_player->playlist_size);
	GA_LoadStream(ga_player->playlist[ga_player->current_track_index]);
	GA_SetTrackEndSync();
	BASS_ChannelPlay(ga_player->mixer_stream, FALSE);
}

void GA_Pause(void) {
	if (ga_player == NULL || ga_player->current_stream == GA_NO_HANDLER || ga_player->playlist == NULL) return;
	BASS_ChannelPause(ga_player->mixer_stream);
}

void GA_Seek(double position) {
	if (ga_player == NULL || ga_player->current_stream == GA_NO_HANDLER) return;

	BASS_Mixer_ChannelSetPosition(ga_player->current_stream,
			BASS_ChannelSeconds2Bytes(ga_player->current_stream, position),
			BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN | BASS_POS_MIXER_RESET);
}

void GA_Next(void) {
	if (ga_player == NULL) return;
	GA_SkipToTrack((int16_t)(ga_player->current_track_index + 1));
}

void GA_Previous(void) {
	if (ga_player == NULL) return;
	GA_SkipToTrack((int16_t)(ga_player->current_track_index - 1));
}

void GA_SetVolume(float volume) {
	if (ga_player == NULL) return;
	BASS_ChannelSetAttribute(ga_player->mixer_stream, BASS_ATTRIB_VOL, volume);
}

float GA_GetVolume(void) {
	if (ga_player == NULL) return 0;

	float volume;
	BASS_ChannelGetAttribute(ga_player->mixer_stream, BASS_ATTRIB_VOL, &volume);
	return BASS_ErrorGetCode() ? 0 : volume;
}

uint16_t GA_GetCurrentTrackIndex(void) {
	if (ga_player == NULL) return 0;
	return ga_player->current_track_index;
}

uint16_t GA_GetPlaylistSize(void) {
	if (ga_player == NULL) return 0;
	return ga_player->playlist_size;
}

enum GA_PlaybackState GA_GetPlaybackState(void) {
	if (ga_player == NULL) return GA_PLAYBACK_STATE_STOPPED;

	uint32_t playback_state = BASS_ChannelIsActive(ga_player->mixer_stream);

	switch (playback_state) {
	case BASS_ACTIVE_PLAYING: return GA_PLAYBACK_STATE_PLAYING;
	case BASS_ACTIVE_PAUSED: return GA_PLAYBACK_STATE_PAUSED;
	default: return GA_PLAYBACK_STATE_STOPPED;
	}

}

double GA_GetTrackPosition(void) {
	if (ga_player == NULL || ga_player->current_stream == GA_NO_HANDLER) return 0;
	QWORD position = BASS_Mixer_ChannelGetPosition(ga_player->current_stream, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(ga_player->current_stream, position);
}

double GA_GetTrackLength(void) {
	if (ga_player == NULL || ga_player->current_stream == GA_NO_HANDLER) return 0;

	QWORD length = BASS_ChannelGetLength(ga_player->current_stream, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(ga_player->current_stream, length);
}
