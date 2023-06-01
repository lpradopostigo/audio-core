#include <windows.h>
#include <stdio.h>
#include "utils.h"
#include "grass_audio.h"

#define TIME_DELTA 1

int tests_run = 0;

const char* playlist[] = {
		CONCAT(PROJECT_TEST_SAMPLE_FILES_DIR, "/01_Ghosts_I.flac"),
		CONCAT(PROJECT_TEST_SAMPLE_FILES_DIR, "/24_Ghosts_III.flac"),
		CONCAT(PROJECT_TEST_SAMPLE_FILES_DIR, "/25_Ghosts_III.flac")
};

const uint16_t playlist_size = sizeof(playlist) / sizeof(playlist[0]);

TEST(basic, {
	ASSERT("init audio device", ga_audio_output_init(44100) == GA_RESULT_OK);

	struct GaPlayer* player = ga_new_player(44100);
	ASSERT("player should not be null", player != NULL);

	ga_free_player(player);

	ASSERT("close audio device", ga_audio_output_close() == GA_RESULT_OK);
})

TEST(basic_playback, {
	ga_audio_output_init(44100);
	struct GaPlayer* player = ga_new_player(44100);

	ga_player_set_sources(player, playlist, 1);

	ASSERT("sources size should be 1", ga_player_get_sources_size(player) == 1);
	ASSERT("source index should be 0", ga_player_get_source_index(player) == 0);
	ASSERT("playback state should be stopped",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_STOPPED);

	ga_player_play(player);

	ASSERT("playback state should be playing",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_PLAYING);

	Sleep(5000);

	ga_player_pause(player);
	ASSERT("playback state should be paused",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_PAUSED);

	Sleep(5000);

	ga_player_play(player);
	ASSERT("playback state should be playing",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_PLAYING);
	Sleep(5000);

	ga_free_player(player);

	ga_audio_output_close();
})

TEST(seek, {
	ga_audio_output_init(44100);
	struct GaPlayer* player = ga_new_player(44100);
	ga_player_set_sources(player, playlist, 1);
	ga_player_play(player);

	ASSERT("playback state should be playing",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_PLAYING);

	Sleep(5000);

	ga_player_seek(player, 60);
	ASSERT("source position should be around 60", ga_player_get_source_position(player) - 60 < TIME_DELTA);
	ASSERT("playback state should be playing",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_PLAYING);

	Sleep(5000);

	ga_player_pause(player);
	ga_player_seek(player, 0);
	ASSERT("playback state should be paused",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_PAUSED);

	ASSERT("source position should be around 0", ga_player_get_source_position(player) < TIME_DELTA);

	Sleep(5000);

	ga_player_play(player);
	ASSERT("playback state should be playing",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_PLAYING);

	Sleep(5000);
	ga_free_player(player);
	ga_audio_output_close();
})

TEST(basic_playlist_playback, {
	ga_audio_output_init(44100);
	struct GaPlayer* player = ga_new_player(44100);

	ga_player_set_sources(player, playlist, playlist_size);
	ASSERT("sources size should be 3", ga_player_get_sources_size(player) == playlist_size);

	ga_player_play(player);
	Sleep(5000);

	ga_player_skip_to(player, 1);
	ASSERT("source index should be 1", ga_player_get_source_index(player) == 1);
	ASSERT("playback state should be playing",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_PLAYING);

	Sleep(5000);

	ga_player_seek(player, 155);
	INFO("transitioning to next audio file");

	Sleep(15000);

	ASSERT("source index should be 2", ga_player_get_source_index(player) == 2);

	ga_audio_output_close();
})

TEST(playlist_end, {
	ga_audio_output_init(44100);
	struct GaPlayer* player = ga_new_player(44100);

	ga_player_set_sources(player, playlist, playlist_size);

	ga_player_play(player);
	ga_player_skip_to(player, 2);
	ASSERT("source index should be 2", ga_player_get_source_index(player) == 2);
	Sleep(5000);

	ga_player_seek(player, 110);
	ASSERT("source position should be around 110", ga_player_get_source_position(player) - 110 < TIME_DELTA);
	Sleep(10000);

	ASSERT("playback state should be stopped",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_STOPPED);
	Sleep(2000);

	ga_player_play(player);
	ASSERT("playback state should be playing",
			ga_player_get_playback_state(player) == GA_PLAYER_PLAYBACK_STATE_PLAYING);
	ASSERT("source index should be 0", ga_player_get_source_index(player) == 0);
	Sleep(5000);

	ga_free_player(player);

	ga_audio_output_close();

})

static char* all_tests(void) {
	RUN_TEST(basic);
	RUN_TEST(basic_playback);
	RUN_TEST(seek);
	RUN_TEST(basic_playlist_playback);
	RUN_TEST(playlist_end);
	return 0;
}

int main(void) {
	char* result = all_tests();
	if (result != 0) {
		printf("[ERROR]: %s\n", result);
	}
	else {
		printf("ALL TESTS PASSED\n");
	}
	printf("Tests run: %d\n", tests_run);
	return result != 0;
}

