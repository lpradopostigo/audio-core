#include "bass.h"
#include "bassmix.h"
#include "utils.h"
#include "grass_audio.h"

void GA_SetTrackEndSync(struct GA_Player* player);
void GA_HandleTrackEndSync(HSYNC handle, DWORD channel, DWORD data, void* user);
void GA_LoadNextTrack(struct GA_Player* player);
void GA_RemoveTrackEndSync(struct GA_Player* player);
void GA_RemoveCurrentStream(struct GA_Player* player);

// only one player is supported at the moment
struct GA_Player* ga_players[] = {NULL};
const char* ga_plugin_path = ".";
HPLUGIN ga_plugins[] = {GA_NO_HANDLER};

int GA_Init(DWORD sample_rate, const char* plugin_path) {
	const char* resolved_plugin_path = plugin_path == NULL ? ga_plugin_path : plugin_path;

	if (ga_plugins[0] == GA_NO_HANDLER) {
		const char* bassflac_path = GA_Concat(resolved_plugin_path, "/bassflac.dll");
		ga_plugins[0] = BASS_PluginLoad(bassflac_path, 0);
		free((void*)bassflac_path);
	}

	BASS_Init(-1, sample_rate, 0, NULL, NULL);
	if (BASS_ErrorGetCode()) {
		return GA_ERROR;
	}

	BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 2000);
	WARN_IF(BASS_ErrorGetCode(), "Failed to set global volume");

	return GA_OK;
}

int GA_Terminate() {
	GA_DestroyPlayer(ga_players[0]);
	BASS_Free();
	return BASS_ErrorGetCode() ? GA_ERROR : GA_OK;
}

struct GA_Player* GA_CreatePlayer(DWORD sample_rate) {
	if (ga_players[0] != NULL) return NULL;

	struct GA_Player* player = (struct GA_Player*)malloc(sizeof(struct GA_Player));

	player->mixer_stream = BASS_Mixer_StreamCreate(sample_rate, 2, BASS_MIXER_END);

	if (BASS_ErrorGetCode()) {
		free(player);
		return NULL;
	}

	player->current_stream = GA_NO_HANDLER;
	player->track_end_sync_handler = GA_NO_HANDLER;
	player->current_track_index = 0;
	player->playlist = NULL;
	player->playlist_size = 0;

	ga_players[0] = player;

	return player;
}

int GA_DestroyPlayer(struct GA_Player* player) {
	if (player == NULL) return GA_OK;

	BASS_StreamFree(player->mixer_stream);
	int error = BASS_ErrorGetCode();

	free((void*)player->playlist);
	free(player);
	ga_players[0] = NULL;

	return error ? GA_ERROR : GA_OK;
}

void GA_Play(struct GA_Player* player) {
	if (player->playlist == NULL) return;

	if (player->current_stream == GA_NO_HANDLER) {
		GA_LoadNextTrack(player);
	}

	BASS_ChannelPlay(player->mixer_stream, FALSE);
}

void GA_Pause(struct GA_Player* player) {
	if (player->current_stream == GA_NO_HANDLER || player->playlist == NULL) return;
	BASS_ChannelPause(player->mixer_stream);
}

void GA_Stop(struct GA_Player* player) {
	GA_RemoveTrackEndSync(player);
	GA_RemoveCurrentStream(player);
	player->current_track_index = 0;
	if (player->playlist != NULL) {
		GA_LoadNextTrack(player);
		GA_SetTrackEndSync(player);
	}
}

void GA_Seek(struct GA_Player* player, double position) {
	if (player->current_stream == GA_NO_HANDLER || player->playlist == NULL) return;

	BASS_Mixer_ChannelSetPosition(player->current_stream,
			BASS_ChannelSeconds2Bytes(player->current_stream, position),
			BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN | BASS_POS_MIXER_RESET);
}

void GA_SkipToTrack(struct GA_Player* player, int index) {
	if (player->playlist == NULL) return;

	GA_RemoveTrackEndSync(player);
	GA_RemoveCurrentStream(player);
	player->current_track_index = GA_ResolveIndex(index, player->playlist_size);
	GA_LoadNextTrack(player);
	GA_SetTrackEndSync(player);
	BASS_ChannelPlay(player->mixer_stream, FALSE);
}

void GA_Next(struct GA_Player* player) {
	GA_SkipToTrack(player, player->current_track_index + 1);
}

void GA_Previous(struct GA_Player* player) {
	GA_SkipToTrack(player, player->current_track_index - 1);
}

void GA_SetPlaylist(struct GA_Player* player, char const* const* playlist, int playlist_size) {
	GA_RemoveTrackEndSync(player);
	GA_RemoveCurrentStream(player);

	wchar_t** new_playlist = (wchar_t**)malloc(sizeof(wchar_t*) * playlist_size);

	for (int i = 0; i < playlist_size; i++) {
		new_playlist[i] = GA_Utf8ToWstring(playlist[i]);
		printf("%ls \n", new_playlist[i]);

	}

	player->playlist = new_playlist;
	player->playlist_size = playlist_size;

	if (playlist_size > 0) {
		GA_LoadNextTrack(player);
		GA_SetTrackEndSync(player);
	}
}

void GA_SetVolume(struct GA_Player* player, float volume) {
	BASS_ChannelSetAttribute(player->mixer_stream, BASS_ATTRIB_VOL, volume);
}

float GA_GetVolume(struct GA_Player* player) {
	float volume;
	BASS_ChannelGetAttribute(player->mixer_stream, BASS_ATTRIB_VOL, &volume);
	return BASS_ErrorGetCode() ? 0 : volume;
}

int GA_GetCurrentTrackIndex(struct GA_Player* player) {
	return player->current_track_index;
}

int GA_GetPlaylistSize(struct GA_Player* player) {
	return player->playlist_size;
}

DWORD GA_GetPlaybackState(struct GA_Player* player) {
	return BASS_ChannelIsActive(player->mixer_stream);
}

double GA_GetTrackPosition(struct GA_Player* player) {
	if (player->current_stream == GA_NO_HANDLER) return 0;
	QWORD position = BASS_Mixer_ChannelGetPosition(player->current_stream, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(player->current_stream, position);
}

double GA_GetTrackLength(struct GA_Player* player) {
	if (player->current_stream == GA_NO_HANDLER) return 0;

	QWORD length = BASS_ChannelGetLength(player->current_stream, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(player->current_stream, length);
}

void GA_RemoveTrackEndSync(struct GA_Player* player) {
	if (player->track_end_sync_handler == GA_NO_HANDLER) return;
	BASS_ChannelRemoveSync(player->mixer_stream, player->track_end_sync_handler);
	player->track_end_sync_handler = GA_NO_HANDLER;
}

void GA_RemoveCurrentStream(struct GA_Player* player) {
	if (player->current_stream == GA_NO_HANDLER) return;
	BASS_Mixer_ChannelRemove(player->current_stream);
	player->current_stream = GA_NO_HANDLER;
}

void GA_LoadNextTrack(struct GA_Player* player) {
	const int remaining_tracks_count = player->playlist_size - player->current_track_index;
	if (remaining_tracks_count == 0) {
		return;
	}

	const wchar_t* current_track = player->playlist[player->current_track_index];
	player->current_stream = BASS_StreamCreateFile(FALSE,
			current_track,
			0,
			0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_UNICODE);

	BASS_Mixer_StreamAddChannel(player->mixer_stream, player->current_stream,
			BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE);
	BASS_ChannelSetPosition(player->mixer_stream, 0, BASS_POS_BYTE);
}

void GA_HandleTrackEndSync(HSYNC handle, DWORD channel, DWORD data, void* user) {
	struct GA_Player* player = ga_players[0];
	player->current_track_index++;
	GA_LoadNextTrack(player);
}

void GA_SetTrackEndSync(struct GA_Player* player) {
	if (player->track_end_sync_handler != GA_NO_HANDLER) return;

	player->track_end_sync_handler = BASS_ChannelSetSync(player->mixer_stream,
			BASS_SYNC_END | BASS_SYNC_MIXTIME | BASS_SYNC_THREAD,
			0,
			&GA_HandleTrackEndSync,
			NULL);
}