#include "bass.h"
#include "bassmix.h"
#include "utils.h"
#include "grass_audio.h"

#define GA_NO_HANDLER 0

struct GA_Player {
  uint32_t mixer_stream;
  uint32_t current_stream;
  uint32_t track_end_sync_handler;
  int current_track_index;
  wchar_t** playlist;
  int playlist_size;
};

void GA_SetTrackEndSync();
void GA_HandleTrackEndSync();
void GA_LoadNextTrack();
void GA_RemoveTrackEndSync();
void GA_RemoveCurrentStream();

static uint32_t ga_plugins[] = {GA_NO_HANDLER};
static struct GA_Player* ga_player = NULL;

#define CHECK_GA_PLAYER_INITIALIZED(return_value) do { if (ga_player == NULL) { printf("GA: grass audio is not initialized"); return return_value; } } while (0)

enum GA_Result GA_Init(uint32_t sample_rate, const char* plugin_path) {
	if (ga_player != NULL) return GA_RESULT_ERROR;

	// init bass
	const char* resolved_plugin_path = plugin_path == NULL ? "." : plugin_path;

	if (ga_plugins[0] == GA_NO_HANDLER) {
		const char* bassflac_path = GA_Concat(resolved_plugin_path, "/bassflac.dll");
		ga_plugins[0] = BASS_PluginLoad(bassflac_path, 0);
		free((void*)bassflac_path);
	}

	if (!BASS_Init(-1, sample_rate, 0, NULL, NULL)) return GA_RESULT_ERROR;
	WARN_IF(!BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 2000), "Failed to set global volume");

	// create player
	ga_player = (struct GA_Player*)malloc(sizeof(struct GA_Player));

	ga_player->mixer_stream = BASS_Mixer_StreamCreate(sample_rate, 2, BASS_MIXER_END);

	if (BASS_ErrorGetCode()) {
		free(ga_player);
		ga_player = NULL;
		return GA_RESULT_ERROR;
	}

	ga_player->current_stream = GA_NO_HANDLER;
	ga_player->track_end_sync_handler = GA_NO_HANDLER;
	ga_player->current_track_index = 0;
	ga_player->playlist = NULL;
	ga_player->playlist_size = 0;

	return GA_RESULT_OK;
}

enum GA_Result GA_Terminate() {
	CHECK_GA_PLAYER_INITIALIZED(GA_RESULT_ERROR);

	// free bass
	if (!BASS_PluginFree(ga_plugins[0])) return GA_RESULT_ERROR;
	ga_plugins[0] = GA_NO_HANDLER;

	if (!BASS_Free()) return GA_RESULT_ERROR;


	// free ga_player
	for (int i = 0; i < ga_player->playlist_size; i++) {
		free((void*)ga_player->playlist[i]);
	}
	free((void*)ga_player->playlist);
	free(ga_player);
	ga_player = NULL;

	return GA_RESULT_OK;

}

void GA_Play() {
	CHECK_GA_PLAYER_INITIALIZED();
	if (ga_player->playlist == NULL) return;

	if (ga_player->current_stream == GA_NO_HANDLER) {
		GA_LoadNextTrack();
	}

	BASS_ChannelPlay(ga_player->mixer_stream, FALSE);
}

void GA_Pause() {
	CHECK_GA_PLAYER_INITIALIZED();
	if (ga_player->current_stream == GA_NO_HANDLER || ga_player->playlist == NULL) return;
	BASS_ChannelPause(ga_player->mixer_stream);
}

void GA_Stop() {
	CHECK_GA_PLAYER_INITIALIZED();
	GA_RemoveTrackEndSync();
	GA_RemoveCurrentStream();
	ga_player->current_track_index = 0;
	if (ga_player->playlist != NULL) {
		GA_LoadNextTrack();
		GA_SetTrackEndSync();
	}
}

void GA_Seek(double position) {
	CHECK_GA_PLAYER_INITIALIZED();
	if (ga_player->current_stream == GA_NO_HANDLER || ga_player->playlist == NULL) return;

	BASS_Mixer_ChannelSetPosition(ga_player->current_stream,
			BASS_ChannelSeconds2Bytes(ga_player->current_stream, position),
			BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN | BASS_POS_MIXER_RESET);
}

void GA_SkipToTrack(int index) {
	CHECK_GA_PLAYER_INITIALIZED();
	if (ga_player->playlist == NULL) return;

	GA_RemoveTrackEndSync();
	GA_RemoveCurrentStream();
	ga_player->current_track_index = GA_ResolveIndex(index, ga_player->playlist_size);
	GA_LoadNextTrack();
	GA_SetTrackEndSync();
	BASS_ChannelPlay(ga_player->mixer_stream, FALSE);
}

void GA_Next() {
	CHECK_GA_PLAYER_INITIALIZED();
	GA_SkipToTrack(ga_player->current_track_index + 1);
}

void GA_Previous() {
	CHECK_GA_PLAYER_INITIALIZED();
	GA_SkipToTrack(ga_player->current_track_index - 1);
}

void GA_SetPlaylist(char const* const* playlist, int playlist_size) {
	CHECK_GA_PLAYER_INITIALIZED();

	GA_RemoveTrackEndSync();
	GA_RemoveCurrentStream();

	wchar_t** new_playlist = (wchar_t**)malloc(sizeof(wchar_t*) * playlist_size);

	for (int i = 0; i < playlist_size; i++) {
		new_playlist[i] = GA_Utf8ToWstring(playlist[i]);
	}

	ga_player->playlist = new_playlist;
	ga_player->playlist_size = playlist_size;

	if (playlist_size > 0) {
		GA_LoadNextTrack();
		GA_SetTrackEndSync();
	}
}

void GA_SetVolume(float volume) {
	CHECK_GA_PLAYER_INITIALIZED();
	BASS_ChannelSetAttribute(ga_player->mixer_stream, BASS_ATTRIB_VOL, volume);
}

float GA_GetVolume() {
	CHECK_GA_PLAYER_INITIALIZED(0);
	float volume;
	BASS_ChannelGetAttribute(ga_player->mixer_stream, BASS_ATTRIB_VOL, &volume);
	return BASS_ErrorGetCode() ? 0 : volume;
}

int GA_GetCurrentTrackIndex() {
	CHECK_GA_PLAYER_INITIALIZED(-1);
	return ga_player->current_track_index;
}

int GA_GetPlaylistSize() {
	CHECK_GA_PLAYER_INITIALIZED(-1);
	return ga_player->playlist_size;
}

enum GA_PlaybackState GA_GetPlaybackState() {
	CHECK_GA_PLAYER_INITIALIZED(GA_PLAYBACK_STATE_STOPPED);
	uint32_t playback_state = BASS_ChannelIsActive(ga_player->mixer_stream);

	switch (playback_state) {
	case BASS_ACTIVE_PLAYING: return GA_PLAYBACK_STATE_PLAYING;
	case BASS_ACTIVE_PAUSED: return GA_PLAYBACK_STATE_PAUSED;
	default: return GA_PLAYBACK_STATE_STOPPED;
	}

}

double GA_GetTrackPosition() {
	CHECK_GA_PLAYER_INITIALIZED(0);
	if (ga_player->current_stream == GA_NO_HANDLER) return 0;
	QWORD position = BASS_Mixer_ChannelGetPosition(ga_player->current_stream, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(ga_player->current_stream, position);
}

double GA_GetTrackLength() {
	CHECK_GA_PLAYER_INITIALIZED(0);
	if (ga_player->current_stream == GA_NO_HANDLER) return 0;

	QWORD length = BASS_ChannelGetLength(ga_player->current_stream, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(ga_player->current_stream, length);
}

void GA_RemoveTrackEndSync() {
	if (ga_player->track_end_sync_handler == GA_NO_HANDLER) return;
	BASS_ChannelRemoveSync(ga_player->mixer_stream, ga_player->track_end_sync_handler);
	ga_player->track_end_sync_handler = GA_NO_HANDLER;
}

void GA_RemoveCurrentStream() {
	if (ga_player->current_stream == GA_NO_HANDLER) return;
	BASS_Mixer_ChannelRemove(ga_player->current_stream);
	ga_player->current_stream = GA_NO_HANDLER;
}

void GA_LoadNextTrack() {
	const int remaining_tracks_count = ga_player->playlist_size - ga_player->current_track_index;
	if (remaining_tracks_count == 0) {
		return;
	}

	const wchar_t* current_track = ga_player->playlist[ga_player->current_track_index];
	ga_player->current_stream = BASS_StreamCreateFile(FALSE,
			current_track,
			0,
			0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_UNICODE);

	BASS_Mixer_StreamAddChannel(ga_player->mixer_stream, ga_player->current_stream,
			BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE);
	BASS_ChannelSetPosition(ga_player->mixer_stream, 0, BASS_POS_BYTE);
}

void GA_HandleTrackEndSync() {
	ga_player->current_track_index++;
	GA_LoadNextTrack();
}

void GA_SetTrackEndSync() {
	if (ga_player->track_end_sync_handler != GA_NO_HANDLER) return;

	ga_player->track_end_sync_handler = BASS_ChannelSetSync(ga_player->mixer_stream,
			BASS_SYNC_END | BASS_SYNC_MIXTIME | BASS_SYNC_THREAD,
			0,
			&GA_HandleTrackEndSync,
			NULL);
}