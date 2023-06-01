#include "ga_player.h"
#include "bass.h"
#include "bassmix.h"
#include <stdbool.h>
#include <stdio.h>

#define UNUSED(x) (void)(x)

void load_stream(struct GaPlayer* player);
void handle_track_end_sync(HSYNC handle, DWORD channel, DWORD data, void* user);

struct GaPlayer* ga_new_player(uint32_t sample_rate) {
	struct GaPlayer* player = (struct GaPlayer*)malloc(sizeof(struct GaPlayer));

	if (player == NULL) return NULL;

	player->mixer_stream_handle = BASS_Mixer_StreamCreate(sample_rate, 2,
			BASS_MIXER_END);

	if (player->mixer_stream_handle == 0) {
		free(player);
		return NULL;
	}

	uint32_t set_sync_result = BASS_ChannelSetSync(player->mixer_stream_handle,
			BASS_SYNC_END | BASS_SYNC_MIXTIME | BASS_SYNC_THREAD, 0,
			&handle_track_end_sync, player);

	if (set_sync_result == 0) {
		BASS_StreamFree(player->mixer_stream_handle);
		free(player);
		return NULL;
	}

	player->stream_handle = 0;
	player->sources = NULL;
	player->source_index = 0;

	return player;
}

void ga_free_player(struct GaPlayer* player) {
	if (player == NULL) return;

	BASS_StreamFree(player->mixer_stream_handle);
	ga_free_source_list(player->sources);
	free(player);
}

enum GaResult ga_player_set_sources(struct GaPlayer* player, const char** sources, size_t sources_size) {
	if (player == NULL) return GA_RESULT_ERROR;

	if (player->stream_handle != 0 && !BASS_Mixer_ChannelRemove(player->stream_handle)) {
		return GA_RESULT_ERROR;
	}

	ga_free_source_list(player->sources);

	player->sources = ga_new_source_list(sources, sources_size);
	player->source_index = 0;
	player->stream_handle = 0;

	return GA_RESULT_OK;
}

void ga_player_play(struct GaPlayer* player) {
	if (player == NULL || player->sources == NULL) return;

	if (player->stream_handle == 0) {
		load_stream(player);
	}

	BASS_ChannelPlay(player->mixer_stream_handle, false);
}

void ga_player_stop(struct GaPlayer* player) {
	if (player == NULL) return;

	BASS_ChannelStop(player->mixer_stream_handle);
	BASS_ChannelSetPosition(player->mixer_stream_handle, 0, BASS_POS_BYTE);

	BASS_Mixer_ChannelRemove(player->stream_handle);

	player->stream_handle = 0;
	player->source_index = 0;
}

void ga_player_pause(struct GaPlayer* player) {
	if (player == NULL) return;

	BASS_ChannelPause(player->mixer_stream_handle);
}

void ga_player_seek(struct GaPlayer* player, double seconds) {
	if (player == NULL || player->stream_handle == 0) return;

	uint64_t position = BASS_ChannelSeconds2Bytes(player->stream_handle, seconds);
	BASS_Mixer_ChannelSetPosition(player->stream_handle, position,
			BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN | BASS_POS_MIXER_RESET);
}

void ga_player_skip_to(struct GaPlayer* player, size_t index) {
	if (player == NULL || player->sources == NULL || index >= player->sources->size) return;

	player->source_index = index;
	load_stream(player);
}

enum GaPlayerPlaybackState ga_player_get_playback_state(struct GaPlayer* player) {
	if (player == NULL) return GA_PLAYER_PLAYBACK_STATE_STOPPED;

	switch (BASS_ChannelIsActive(player->mixer_stream_handle)) {
	case BASS_ACTIVE_PLAYING: return GA_PLAYER_PLAYBACK_STATE_PLAYING;
	case BASS_ACTIVE_PAUSED: return GA_PLAYER_PLAYBACK_STATE_PAUSED;
	default: return GA_PLAYER_PLAYBACK_STATE_STOPPED;
	}
}

double ga_player_get_source_position(struct GaPlayer* player) {
	if (player == NULL || player->stream_handle == 0) return 0;

	uint64_t position = BASS_ChannelGetPosition(player->stream_handle, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(player->stream_handle, position);
}

double ga_player_get_source_duration(struct GaPlayer* player) {
	if (player == NULL || player->stream_handle == 0) return 0;

	uint64_t length = BASS_ChannelGetLength(player->stream_handle, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(player->stream_handle, length);
}

float ga_player_get_source_volume(struct GaPlayer* player) {
	if (player == NULL || player->stream_handle == 0) return 0;

	float volume;
	BASS_ChannelGetAttribute(player->stream_handle, BASS_ATTRIB_VOL, &volume);
	return volume;
}

void ga_player_set_source_volume(struct GaPlayer* player, float volume) {
	if (player == NULL || player->stream_handle == 0) return;

	BASS_ChannelSetAttribute(player->stream_handle, BASS_ATTRIB_VOL, volume);
}

const char* ga_player_get_source_path(struct GaPlayer* player) {
	if (player == NULL || player->sources == NULL || player->stream_handle == 0) return NULL;

	return player->sources->list[player->source_index]->path;
}

size_t ga_player_get_source_index(struct GaPlayer* player) {
	if (player == NULL || player->sources == NULL || player->stream_handle == 0) return 0;

	return player->source_index;
}

size_t ga_player_get_sources_size(struct GaPlayer* player) {
	if (player == NULL || player->sources == NULL) return 0;

	return player->sources->size;
}

void load_stream(struct GaPlayer* player) {
	if (player->sources == NULL) return;

	BASS_Mixer_ChannelRemove(player->stream_handle);

	struct GaSource* source = player->sources->list[player->source_index];

	player->stream_handle = BASS_StreamCreateFile(FALSE,
			source->wpath,
			0,
			0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_UNICODE);

	BASS_Mixer_StreamAddChannel(player->mixer_stream_handle, player->stream_handle,
			BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE);

	BASS_ChannelSetPosition(player->mixer_stream_handle, 0, BASS_POS_BYTE);
}

void handle_track_end_sync(HSYNC handle, DWORD channel, DWORD data, void* user) {
	UNUSED(handle);
	UNUSED(channel);
	UNUSED(data);

	struct GaPlayer* player = (struct GaPlayer*)user;

	if (player->source_index == player->sources->size - 1) {
		player->source_index = 0;
		player->stream_handle = 0;
		return;
	}

	player->source_index++;
	load_stream(player);
}