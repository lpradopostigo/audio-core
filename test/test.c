#include "grass_audio.h"
#include <windows.h>
#include "utils.h"
#include <stdio.h>

int tests_run = 0;

const char* playlist[] = {
		CONCAT(PROJECT_TEST_SAMPLE_FILES_DIR, "/01_Ghosts_I.flac"),
		CONCAT(PROJECT_TEST_SAMPLE_FILES_DIR, "/24_Ghosts_III.flac"),
		CONCAT(PROJECT_TEST_SAMPLE_FILES_DIR, "/25_Ghosts_III.flac")
};

const uint16_t playlist_size = 3;

TEST(basic, {
	ASSERT("init", ga_init(44100) == GA_RESULT_OK);
	ASSERT("terminate", ga_terminate() == GA_RESULT_OK);
})

TEST(basic_playback, {
	ASSERT("init", ga_init(44100) == GA_RESULT_OK);

	ga_set_playlist(playlist, playlist_size);
	ASSERT("set playlist", ga_get_playlist_size() == playlist_size);

	ga_play();
	ASSERT("play", ga_get_playback_state() == GA_PLAYBACK_STATE_PLAYING);

	INFO("playing audio for 5 seconds");
	Sleep(5000);

	ga_pause();
	ASSERT("pause", ga_get_playback_state() == GA_PLAYBACK_STATE_PAUSED);
	INFO("pausing audio for 5 seconds");
	Sleep(5000);

	ga_skip_to_track(1);
	ASSERT("skip to track 1", ga_get_current_track_index() == 1);
	INFO("playing audio after skip for 5 seconds");
	Sleep(5000);

	ga_pause();
	ASSERT("pause", ga_get_playback_state() == GA_PLAYBACK_STATE_PAUSED);
	INFO("pausing audio for 5 seconds");
	Sleep(5000);

	ga_seek(155);
	ASSERT("seek", ga_get_track_position() == 155);
	ga_play();
	INFO("playing audio after seek for 15 seconds, no gaps should be heard");
	Sleep(15000);

	ASSERT("terminate", ga_terminate() == GA_RESULT_OK);
})

TEST(playlist_end, {
	ASSERT("init", ga_init(44100) == GA_RESULT_OK);

	ga_set_playlist(playlist, playlist_size);
	ASSERT("set playlist", ga_get_playlist_size() == playlist_size);
	ASSERT("current track index", ga_get_current_track_index() == 0);

	ga_play();
	ASSERT("play", ga_get_playback_state() == GA_PLAYBACK_STATE_PLAYING);

	INFO("playing audio for 5 seconds");
	Sleep(5000);

	ga_skip_to_track(2);
	ASSERT("skip to track 2", ga_get_current_track_index() == 2);
	Sleep(5000);

	ga_seek(110);
	ASSERT("seek to 110", ga_get_track_position() == 110);

	Sleep(10000);
	ASSERT("stopped", ga_get_playback_state() == GA_PLAYBACK_STATE_STOPPED);
	ASSERT("current track index 0", ga_get_current_track_index() == 0);

	ASSERT("terminate", ga_terminate() == GA_RESULT_OK);
})

static char* all_tests(void) {
	RUN_TEST(basic);
	RUN_TEST(basic_playback);
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

