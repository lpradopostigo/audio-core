#include "gp_player.h"
#include "bass.h"
#include "bassmix.h"
#include <stdbool.h>
#include <stdio.h>
#include "gp_audio_output.h"

static struct GpPlayer* player = NULL;

void load_stream(void);
void handle_track_end_sync(void);

enum GpResult gp_init(enum GpSampleRate sample_rate) {
	if (player != NULL) return GP_RESULT_ERROR;

	if (gp_audio_output_init(sample_rate) != GP_RESULT_OK) {
		return GP_RESULT_ERROR;
	}

	player = (struct GpPlayer*)malloc(sizeof(struct GpPlayer));

	player->mixer_stream_handle = BASS_Mixer_StreamCreate(sample_rate, 2,
			BASS_MIXER_END);

	if (player->mixer_stream_handle == 0) {
		free(player);
		player = NULL;
		return GP_RESULT_ERROR;
	}

	uint32_t set_sync_result = BASS_ChannelSetSync(player->mixer_stream_handle,
			BASS_SYNC_END | BASS_SYNC_MIXTIME | BASS_SYNC_THREAD, 0,
			(void (*)(HSYNC, DWORD, DWORD, void*))&handle_track_end_sync, player);

	if (set_sync_result == 0) {
		BASS_StreamFree(player->mixer_stream_handle);
		free(player);
		player = NULL;
		return GP_RESULT_ERROR;
	}

	player->stream_handle = 0;
	player->sources = NULL;
	player->source_index = 0;

	return GP_RESULT_OK;
}

enum GpResult gp_close(void) {
	if (player == NULL) return GP_RESULT_OK;

	if (!BASS_StreamFree(player->mixer_stream_handle)) {
		return GP_RESULT_ERROR;
	}
	gp_free_source_list(player->sources);
	free(player);

	if (gp_audio_output_close() != GP_RESULT_OK) {
		return GP_RESULT_ERROR;
	}

	player = NULL;

	return GP_RESULT_OK;
}

enum GpResult gp_set_sources(const char** sources, size_t sources_size) {
	if (player == NULL) return GP_RESULT_ERROR;

	if (player->stream_handle != 0 && !BASS_Mixer_ChannelRemove(player->stream_handle)) {
		return GP_RESULT_ERROR;
	}

	gp_free_source_list(player->sources);

	player->sources = gp_new_source_list(sources, sources_size);
	player->source_index = 0;
	player->stream_handle = 0;

	return GP_RESULT_OK;
}

void gp_play(void) {
	if (player == NULL || player->sources == NULL) return;

	if (player->stream_handle == 0) {
		load_stream();
	}

	BASS_ChannelPlay(player->mixer_stream_handle, false);

}

void gp_stop(void) {
	if (player == NULL) return;

	BASS_ChannelStop(player->mixer_stream_handle);
	BASS_ChannelSetPosition(player->mixer_stream_handle, 0, BASS_POS_BYTE);

	BASS_Mixer_ChannelRemove(player->stream_handle);

	player->stream_handle = 0;
	player->source_index = 0;
}

void gp_pause(void) {
	if (player == NULL) return;

	BASS_ChannelPause(player->mixer_stream_handle);
}

void gp_seek(double seconds) {
	if (player == NULL || player->stream_handle == 0) return;

	uint64_t position = BASS_ChannelSeconds2Bytes(player->stream_handle, seconds);
	BASS_Mixer_ChannelSetPosition(player->stream_handle, position,
			BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN | BASS_POS_MIXER_RESET);
}

void gp_skip_to(size_t source_index) {
	if (player == NULL || player->sources == NULL || source_index >= player->sources->size) return;

	player->source_index = source_index;
	load_stream();
}

enum GpPlaybackState gp_get_playback_state(void) {
	if (player == NULL) return GP_PLAYBACK_STATE_STOPPED;

	switch (BASS_ChannelIsActive(player->mixer_stream_handle)) {
	case BASS_ACTIVE_PLAYING: return GP_PLAYBACK_STATE_PLAYING;
	case BASS_ACTIVE_PAUSED: return GP_PLAYBACK_STATE_PAUSED;
	default: return GP_PLAYBACK_STATE_STOPPED;
	}
}

double gp_get_source_position(void) {
	if (player == NULL || player->stream_handle == 0) return 0;

	uint64_t position = BASS_ChannelGetPosition(player->stream_handle, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(player->stream_handle, position);
}

double gp_get_source_duration(void) {
	if (player == NULL || player->stream_handle == 0) return 0;

	uint64_t length = BASS_ChannelGetLength(player->stream_handle, BASS_POS_BYTE);
	return BASS_ChannelBytes2Seconds(player->stream_handle, length);
}

float gp_get_volume(void) {
	if (player == NULL || player->stream_handle == 0) return 0;

	float volume;
	BASS_ChannelGetAttribute(player->stream_handle, BASS_ATTRIB_VOL, &volume);
	return volume;
}

void gp_set_volume(float volume) {
	if (player == NULL || player->stream_handle == 0) return;

	BASS_ChannelSetAttribute(player->stream_handle, BASS_ATTRIB_VOL, volume);
}

const char* gp_get_source_path(void) {
	if (player == NULL || player->sources == NULL || player->stream_handle == 0) return NULL;

	return player->sources->list[player->source_index]->path;
}

size_t gp_get_source_index(void) {
	if (player == NULL || player->sources == NULL || player->stream_handle == 0) return 0;

	return player->source_index;
}

size_t gp_get_sources_size(void) {
	if (player == NULL || player->sources == NULL) return 0;

	return player->sources->size;
}

void load_stream(void) {
	if (player->sources == NULL) return;

	BASS_Mixer_ChannelRemove(player->stream_handle);

	struct GpSource* source = player->sources->list[player->source_index];

	player->stream_handle = BASS_StreamCreateFile(FALSE,
			source->wpath,
			0,
			0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_UNICODE);

	BASS_Mixer_StreamAddChannel(player->mixer_stream_handle, player->stream_handle,
			BASS_MIXER_NORAMPIN | BASS_STREAM_AUTOFREE);

	BASS_ChannelSetPosition(player->mixer_stream_handle, 0, BASS_POS_BYTE);
}

void handle_track_end_sync(void) {
	if (player->source_index == player->sources->size - 1) {
		player->source_index = 0;
		player->stream_handle = 0;
		return;
	}

	player->source_index++;
	load_stream();
}
