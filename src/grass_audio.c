#include <windows.h>
#include "bass.h"
#include "bassmix.h"
#include "grass_audio.h"

wchar_t* utf_8_to_utf_16(const char* utf8);
char* utf_16_to_utf_8(const wchar_t* utf16);
uint16_t resolve_index(int16_t index, uint16_t size);
void set_track_end_sync(void);
void handle_track_end_sync(void);
void load_stream(wchar_t* path);

struct Player {
  uint32_t stream;
  uint32_t mixer_stream;
  uint32_t track_end_sync_handler;
  uint32_t sample_rate;
  wchar_t** playlist;
  uint16_t playlist_size;
  uint16_t playlist_index;
};

static uint32_t ga_plugins[] = {0};
static struct Player* ga_player = NULL;

enum GAResult ga_init(uint32_t sample_rate) {
	// don't init twice
	if (ga_player != NULL) return GA_RESULT_ERROR;

	// load BASS plugins
	if (ga_plugins[0] == 0) {
		const char* bassflac_path = "./bassflac.dll";
		ga_plugins[0] = BASS_PluginLoad(bassflac_path, 0);
	}

	// init bass
	if (!BASS_Init(-1, sample_rate, 0, NULL, NULL)) return GA_RESULT_ERROR;

	// init player
	ga_player = (struct Player*)malloc(sizeof(struct Player));
	ga_player->sample_rate = sample_rate;
	ga_player->stream = 0;
	ga_player->mixer_stream = 0;
	ga_player->playlist = NULL;
	ga_player->playlist_size = 0;
	ga_player->playlist_index = 0;
	ga_player->track_end_sync_handler = 0;

	return GA_RESULT_OK;
}

enum GAResult ga_terminate(void) {
	// don't terminate twice
	if (ga_player == NULL) return GA_RESULT_ERROR;

	// free bass plugins
	if (!BASS_PluginFree(ga_plugins[0])) return GA_RESULT_ERROR;
	ga_plugins[0] = 0;

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

void ga_set_playlist(char const* const* playlist, uint16_t playlist_size) {
	if (ga_player == NULL) return;

	// free old mixer stream and its resources
	if (ga_player->mixer_stream != 0) {
		BASS_StreamFree(ga_player->mixer_stream);
		ga_player->stream = 0;
		ga_player->track_end_sync_handler = 0;
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
		new_playlist[i] = utf_8_to_utf_16(playlist[i]);
	}

	// replace playlist
	ga_player->playlist = new_playlist;
	ga_player->playlist_size = playlist_size;
	ga_player->playlist_index = 0;
}

void ga_play(void) {
	if (ga_player == NULL || ga_player->playlist == NULL) return;

	if (ga_player->stream == 0) {
		load_stream(ga_player->playlist[ga_player->playlist_index]);
		set_track_end_sync();
	}

	BASS_ChannelPlay(ga_player->mixer_stream, FALSE);
}

void ga_stop(void) {
	if (ga_player == NULL) return;

	// free the mixer and its resources
	if (ga_player->mixer_stream != 0) {
		BASS_StreamFree(ga_player->mixer_stream);
		ga_player->mixer_stream = 0;
		ga_player->stream = 0;
		ga_player->track_end_sync_handler = 0;
	}

	ga_player->playlist_index = 0;
}

void ga_skip_to_track(int16_t index) {
	if (ga_player == NULL || ga_player->playlist == NULL) return;
	BASS_ChannelRemoveSync(ga_player->mixer_stream, ga_player->track_end_sync_handler);
	ga_player->track_end_sync_handler = 0;
	BASS_Mixer_ChannelRemove(ga_player->stream);
	ga_player->stream = 0;
	ga_player->playlist_index = resolve_index(index, ga_player->playlist_size);
	load_stream(ga_player->playlist[ga_player->playlist_index]);
	set_track_end_sync();
	BASS_ChannelPlay(ga_player->mixer_stream, FALSE);
}

void ga_pause(void) {
	if (ga_player == NULL || ga_player->stream == 0 || ga_player->playlist == NULL) return;
	BASS_ChannelPause(ga_player->mixer_stream);
}

void ga_seek(double position) {
	if (ga_player == NULL || ga_player->stream == 0) return;

	BASS_Mixer_ChannelSetPosition(ga_player->stream,
			BASS_ChannelSeconds2Bytes(ga_player->stream, position),
			BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN | BASS_POS_MIXER_RESET);
}

void ga_next(void) {
	if (ga_player == NULL) return;
	ga_skip_to_track((int16_t)(ga_player->playlist_index + 1));
}

void ga_previous(void) {
	if (ga_player == NULL) return;
	ga_skip_to_track((int16_t)(ga_player->playlist_index - 1));
}

void ga_set_volume(float volume) {
	if (ga_player == NULL) return;
	BASS_ChannelSetAttribute(ga_player->mixer_stream, BASS_ATTRIB_VOL, volume);
}

float ga_get_volume(void) {
	if (ga_player == NULL) return 0;

	float volume;
	BASS_ChannelGetAttribute(ga_player->mixer_stream, BASS_ATTRIB_VOL, &volume);
	return BASS_ErrorGetCode() ? 0 : volume;
}

uint16_t ga_get_current_track_index(void) {
	if (ga_player == NULL) return 0;
	return ga_player->playlist_index;
}

const char* ga_get_current_track_path(void) {
	if (ga_player == NULL || ga_player->playlist == NULL) return NULL;
	const char* path = utf_16_to_utf_8(ga_player->playlist[ga_player->playlist_index]);
	return path;
}

uint16_t ga_get_playlist_size(void) {
	if (ga_player == NULL) return 0;
	return ga_player->playlist_size;
}

enum GAPlaybackState ga_get_playback_state(void) {
	if (ga_player == NULL) return GA_PLAYBACK_STATE_STOPPED;

	uint32_t playback_state = BASS_ChannelIsActive(ga_player->mixer_stream);

	switch (playback_state) {
	case BASS_ACTIVE_PLAYING: return GA_PLAYBACK_STATE_PLAYING;
	case BASS_ACTIVE_PAUSED: return GA_PLAYBACK_STATE_PAUSED;
	default: return GA_PLAYBACK_STATE_STOPPED;
	}

}

double ga_get_track_position(void) {
	if (ga_player == NULL || ga_player->stream == 0) return 0;
	QWORD position = BASS_Mixer_ChannelGetPosition(ga_player->stream, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(ga_player->stream, position);
}

double ga_get_track_length(void) {
	if (ga_player == NULL || ga_player->stream == 0) return 0;

	QWORD length = BASS_ChannelGetLength(ga_player->stream, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(ga_player->stream, length);
}

void set_track_end_sync(void) {
	ga_player->track_end_sync_handler = BASS_ChannelSetSync(ga_player->mixer_stream,
			BASS_SYNC_END | BASS_SYNC_MIXTIME | BASS_SYNC_THREAD,
			0,
			(void (*)(HSYNC, DWORD, DWORD, void*))&handle_track_end_sync,
			NULL);
}

void load_stream(wchar_t* path) {
	ga_player->stream = BASS_StreamCreateFile(FALSE,
			path,
			0,
			0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_UNICODE);

	BASS_Mixer_StreamAddChannel(ga_player->mixer_stream, ga_player->stream,
			BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE);
	BASS_ChannelSetPosition(ga_player->mixer_stream, 0, BASS_POS_BYTE);
}

void handle_track_end_sync(void) {
	if (ga_player->playlist_index == ga_player->playlist_size - 1) {
		ga_player->playlist_index = 0;
		return;
	};

	ga_player->playlist_index++;
	load_stream(ga_player->playlist[ga_player->playlist_index]);
}

wchar_t* utf_8_to_utf_16(const char* utf8) {
	const int utf8_length = (int)strlen(utf8);
	const int wstr_length = MultiByteToWideChar(CP_UTF8, 0, utf8, utf8_length, NULL, 0);

	wchar_t* wstr = (wchar_t*)malloc(sizeof(wchar_t) * (wstr_length + 1));

	MultiByteToWideChar(CP_UTF8, 0, utf8, utf8_length, wstr, wstr_length);
	wstr[wstr_length] = L'\0';

	return wstr;
}

char* utf_16_to_utf_8(const wchar_t* utf16) {
	const int utf16_length = (int)wcslen(utf16);
	const int str_length = WideCharToMultiByte(CP_UTF8, 0, utf16, utf16_length, NULL, 0, NULL, NULL);

	char* str = (char*)malloc(sizeof(char) * (str_length + 1));

	WideCharToMultiByte(CP_UTF8, 0, utf16, utf16_length, str, str_length, NULL, NULL);
	str[str_length] = '\0';

	return str;
}

uint16_t resolve_index(int16_t index, uint16_t size) {
	if (index < 0) {
		return size - 1;
	}
	else if (index >= size) {
		return 0;
	}
	else {
		return index;
	}
}

