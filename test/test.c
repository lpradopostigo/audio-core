#include <windows.h>
#include <stdio.h>
#include "utils.h"
#include "grass_player.h"

#define TIME_DELTA 1

int tests_run = 0;

const char* playlist[] = {
		CONCAT(PROJECT_TEST_DIR, "/sample-files/01_Ghosts_I.flac"),
		CONCAT(PROJECT_TEST_DIR, "/sample-files/24_Ghosts_III.flac"),
		CONCAT(PROJECT_TEST_DIR, "/sample-files/25_Ghosts_III.flac")
};

const uint16_t playlist_size = sizeof(playlist) / sizeof(playlist[0]);

TEST(basic, {
	ASSERT("init", gp_init(GP_SAMPLE_RATE_44100) == GP_RESULT_OK);
	ASSERT("close", gp_close() == GP_RESULT_OK);
})

TEST(basic_playback, {
	gp_init(GP_SAMPLE_RATE_44100);

	gp_set_sources(playlist, 1);

	ASSERT("sources size should be 1", gp_get_sources_size() == 1);
	ASSERT("source index should be 0", gp_get_source_index() == 0);
	ASSERT("playback state should be stopped",
			gp_get_playback_state() == GP_PLAYBACK_STATE_STOPPED);

	gp_play();

	ASSERT("playback state should be playing",
			gp_get_playback_state() == GP_PLAYBACK_STATE_PLAYING);

	Sleep(5000);

	gp_pause();
	ASSERT("playback state should be paused",
			gp_get_playback_state() == GP_PLAYBACK_STATE_PAUSED);

	Sleep(5000);

	gp_play();
	ASSERT("playback state should be playing",
			gp_get_playback_state() == GP_PLAYBACK_STATE_PLAYING);
	Sleep(5000);

	gp_close();
})

TEST(seek, {
	gp_init(GP_SAMPLE_RATE_44100);
	gp_set_sources(playlist, 1);
	gp_play();

	ASSERT("playback state should be playing",
			gp_get_playback_state() == GP_PLAYBACK_STATE_PLAYING);

	Sleep(5000);

	gp_seek(60);
	ASSERT("source position should be around 60", gp_get_source_position() - 60 < TIME_DELTA);
	ASSERT("playback state should be playing",
			gp_get_playback_state() == GP_PLAYBACK_STATE_PLAYING);

	Sleep(5000);

	gp_pause();
	gp_seek(0);
	ASSERT("playback state should be paused",
			gp_get_playback_state() == GP_PLAYBACK_STATE_PAUSED);

	ASSERT("source position should be around 0", gp_get_source_position() < TIME_DELTA);

	Sleep(5000);

	gp_play();
	ASSERT("playback state should be playing",
			gp_get_playback_state() == GP_PLAYBACK_STATE_PLAYING);

	Sleep(5000);
	gp_close();
})

TEST(basic_playlist_playback, {
	gp_init(GP_SAMPLE_RATE_44100);

	gp_set_sources(playlist, playlist_size);
	ASSERT("sources size should be 3", gp_get_sources_size() == playlist_size);

	gp_play();
	Sleep(5000);

	gp_skip_to(1);
	ASSERT("source index should be 1", gp_get_source_index() == 1);
	ASSERT("playback state should be playing",
			gp_get_playback_state() == GP_PLAYBACK_STATE_PLAYING);

	Sleep(5000);

	gp_seek(155);
	INFO("transitioning to next audio file");

	Sleep(15000);

	ASSERT("source index should be 2", gp_get_source_index() == 2);

	gp_close();
})

TEST(playlist_end, {
	gp_init(GP_SAMPLE_RATE_44100);

	gp_set_sources(playlist, playlist_size);

	gp_play();
	gp_skip_to(2);
	ASSERT("source index should be 2", gp_get_source_index() == 2);
	Sleep(5000);

	gp_seek(110);
	ASSERT("source position should be around 110", gp_get_source_position() - 110 < TIME_DELTA);
	Sleep(10000);

	ASSERT("playback state should be stopped",
			gp_get_playback_state() == GP_PLAYBACK_STATE_STOPPED);
	Sleep(2000);

	gp_play();
	ASSERT("playback state should be playing",
			gp_get_playback_state() == GP_PLAYBACK_STATE_PLAYING);
	ASSERT("source index should be 0", gp_get_source_index() == 0);
	Sleep(5000);

	gp_close();

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

