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
	ASSERT("playlist size should match", ga_get_playlist_size() == playlist_size);

	ga_play();
	ASSERT("playback state should be playing", ga_get_playback_state() == GA_PLAYBACK_STATE_PLAYING);
	Sleep(5000);

	ga_pause();
	ASSERT("playback state should be paused", ga_get_playback_state() == GA_PLAYBACK_STATE_PAUSED);
	Sleep(5000);

	ga_skip_to(1);
	ASSERT("playlist index should be 1", ga_get_playlist_index() == 1);
	Sleep(5000);

	ga_seek(155);
	ASSERT("playback position should be 155", ga_get_position() == 155);

	ga_play();
	ASSERT("playback state should be playing", ga_get_playback_state() == GA_PLAYBACK_STATE_PLAYING);
	INFO("transitioning to next audio file, no gaps should be heard");
	Sleep(15000);

	ASSERT("terminate", ga_terminate() == GA_RESULT_OK);
})

TEST(playlist_end, {
	ASSERT("init", ga_init(44100) == GA_RESULT_OK);

	ga_set_playlist(playlist, playlist_size);
	ASSERT("current playlist index should be 0", ga_get_playlist_index() == 0);

	ga_play();
	INFO("playing audio for 5 seconds");
	ASSERT("playback state should be playing", ga_get_playback_state() == GA_PLAYBACK_STATE_PLAYING);
	Sleep(5000);

	ga_skip_to(2);
	ASSERT("current playlist index should be 2", ga_get_playlist_index() == 2);
	Sleep(5000);

	ga_seek(110);
	ASSERT("current position should be 110", ga_get_position() == 110);
	Sleep(10000);

	ASSERT("playback state should be stopped", ga_get_playback_state() == GA_PLAYBACK_STATE_STOPPED);
	Sleep(2000);

	ga_play();
	INFO("playing audio for 5 seconds after playlist end");
	ASSERT("current playlist index should be 0", ga_get_playlist_index() == 0);
	Sleep(5000);

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

