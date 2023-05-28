#include <windows.h>
#include "bass.h"
#include "bassmix.h"
#include "grass_audio.h"

wchar_t* utf_8_to_utf_16(const char* utf8);
char* utf_16_to_utf_8(const wchar_t* utf16);
uint16_t resolve_index(int16_t index, uint16_t size);
void handle_track_end_sync(void);
void load_stream(wchar_t* path);

struct Player {
  uint32_t stream;
  uint32_t mixer_stream;
  uint32_t sample_rate;
  wchar_t** playlist;
  uint16_t playlist_size;
  uint16_t playlist_index;
};

struct Plugin {
  const char* path;
  uint32_t handle;
};

static struct Plugin plugins[] = {
		{"./bassflac.dll", 0}
};

static uint8_t plugins_size = sizeof(plugins) / sizeof(struct Plugin);

static struct Player* player = NULL;

enum GAResult ga_init(uint32_t sample_rate) {
	if (player) return GA_RESULT_ERROR;

	for (uint8_t i = 0; i < plugins_size; i++) {
		plugins[i].handle = BASS_PluginLoad(plugins[i].path, 0);
		if (plugins[i].handle == 0) return GA_RESULT_ERROR;
	}

	if (!BASS_Init(-1, sample_rate, 0, NULL, NULL)) return GA_RESULT_ERROR;

	player = (struct Player*)malloc(sizeof(struct Player));
	player->sample_rate = sample_rate;
	player->stream = 0;
	player->mixer_stream = 0;
	player->playlist = NULL;
	player->playlist_size = 0;
	player->playlist_index = 0;

	return GA_RESULT_OK;
}

enum GAResult ga_terminate(void) {
	if (!player) return GA_RESULT_ERROR;

	for (uint8_t i = 0; i < plugins_size; i++) {
		if (!BASS_PluginFree(plugins[i].handle)) return GA_RESULT_ERROR;
		plugins[i].handle = 0;
	}

	if (!BASS_Free()) return GA_RESULT_ERROR;

	for (uint16_t i = 0; i < player->playlist_size; i++) {
		free((void*)player->playlist[i]);
	}
	free((void*)player->playlist);
	free(player);
	player = NULL;

	return GA_RESULT_OK;
}

void ga_set_playlist(char const* const* playlist, uint16_t playlist_size) {
	if (!player) return;

	if (player->mixer_stream) {
		BASS_StreamFree(player->mixer_stream);
		player->stream = 0;
		player->mixer_stream = 0;
	}

	if (player->playlist != NULL) {
		for (uint16_t i = 0; i < player->playlist_size; i++) {
			free((void*)player->playlist[i]);
		}
		free((void*)player->playlist);
	}

	wchar_t** new_playlist = (wchar_t**)malloc(sizeof(wchar_t*) * playlist_size);

	for (uint16_t i = 0; i < playlist_size; i++) {
		new_playlist[i] = utf_8_to_utf_16(playlist[i]);
	}

	player->playlist = new_playlist;
	player->playlist_size = playlist_size;
	player->playlist_index = 0;
}

void ga_play(void) {
	if (!player || !player->playlist) return;

	if (!player->mixer_stream) {
		player->mixer_stream = BASS_Mixer_StreamCreate(player->sample_rate, 2, BASS_MIXER_END | BASS_STREAM_AUTOFREE);
		BASS_ChannelSetSync(player->mixer_stream,
				BASS_SYNC_END | BASS_SYNC_MIXTIME | BASS_SYNC_THREAD,
				0,
				(void (*)(HSYNC, DWORD, DWORD, void*))&handle_track_end_sync,
				NULL);
		load_stream(player->playlist[player->playlist_index]);
	}

	BASS_ChannelPlay(player->mixer_stream, FALSE);
}

void ga_pause(void) {
	if (!player || !player->playlist || !player->mixer_stream) return;
	BASS_ChannelPause(player->mixer_stream);
}

void ga_stop(void) {
	if (!player) return;

	if (player->mixer_stream) {
		BASS_StreamFree(player->mixer_stream);
		player->mixer_stream = 0;
		player->stream = 0;
	}

	player->playlist_index = 0;
}

void ga_skip_to(int16_t index) {
	if (!player || !player->playlist) return;

	enum GAPlaybackState playback_state = ga_get_playback_state();

	player->playlist_index = resolve_index(index, player->playlist_size);

	if (playback_state == GA_PLAYBACK_STATE_STOPPED) {
		return;
	}

	BASS_Mixer_ChannelRemove(player->stream);
	load_stream(player->playlist[player->playlist_index]);

	if (playback_state == GA_PLAYBACK_STATE_PLAYING) {
		BASS_ChannelPlay(player->mixer_stream, FALSE);
	}

}

void ga_seek(double position) {
	if (!player || !player->stream) return;

	BASS_Mixer_ChannelSetPosition(player->stream,
			BASS_ChannelSeconds2Bytes(player->stream, position),
			BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN | BASS_POS_MIXER_RESET);
}

void ga_next(void) {
	if (!player) return;
	ga_skip_to((int16_t)(player->playlist_index + 1));
}

void ga_previous(void) {
	if (!player) return;
	ga_skip_to((int16_t)(player->playlist_index - 1));
}

void ga_set_volume(float volume) {
	if (!player) return;
	BASS_ChannelSetAttribute(player->mixer_stream, BASS_ATTRIB_VOL, volume);
}

float ga_get_volume(void) {
	if (!player) return 0;

	float volume;
	BASS_ChannelGetAttribute(player->mixer_stream, BASS_ATTRIB_VOL, &volume);
	return BASS_ErrorGetCode() ? 0 : volume;
}

uint16_t ga_get_playlist_index(void) {
	if (!player) return 0;
	return player->playlist_index;
}

const char* ga_get_playlist_path(void) {
	if (!player || !player->playlist) return NULL;
	const char* path = utf_16_to_utf_8(player->playlist[player->playlist_index]);
	return path;
}

uint16_t ga_get_playlist_size(void) {
	if (!player) return 0;
	return player->playlist_size;
}

enum GAPlaybackState ga_get_playback_state(void) {
	if (!player || !player->mixer_stream) return GA_PLAYBACK_STATE_STOPPED;

	uint32_t playback_state = BASS_ChannelIsActive(player->mixer_stream);

	switch (playback_state) {
	case BASS_ACTIVE_PLAYING: return GA_PLAYBACK_STATE_PLAYING;
	case BASS_ACTIVE_PAUSED: return GA_PLAYBACK_STATE_PAUSED;
	default: return GA_PLAYBACK_STATE_STOPPED;
	}

}

double ga_get_position(void) {
	if (!player || !player->stream) return 0;
	QWORD position = BASS_Mixer_ChannelGetPosition(player->stream, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(player->stream, position);
}

double ga_get_length(void) {
	if (!player || !player->stream) return 0;

	QWORD length = BASS_ChannelGetLength(player->stream, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(player->stream, length);
}

void load_stream(wchar_t* path) {
	player->stream = BASS_StreamCreateFile(FALSE,
			path,
			0,
			0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_UNICODE);

	BASS_Mixer_StreamAddChannel(player->mixer_stream, player->stream,
			BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE);
	BASS_ChannelSetPosition(player->mixer_stream, 0, BASS_POS_BYTE);
}

void handle_track_end_sync(void) {
	if (player->playlist_index == player->playlist_size - 1) {
		player->playlist_index = 0;
		player->stream = 0;
		player->mixer_stream = 0;
		return;
	};

	player->playlist_index++;
	load_stream(player->playlist[player->playlist_index]);
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

