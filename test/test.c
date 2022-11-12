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

const int playlist_size = 3;

TEST(basic, {
	ASSERT("init", GA_Init(44100, NULL) == GA_OK);
	ASSERT("terminate", GA_Terminate() == GA_OK);
})

TEST(basic_playback, {
	ASSERT("init", GA_Init(44100, NULL) == GA_OK);

	GA_SetPlaylist(playlist, playlist_size);
	ASSERT("set playlist", GA_GetPlaylistSize() == playlist_size);

	GA_Play();
	ASSERT("play", GA_GetPlaybackState() == GA_PLAYBACK_STATE_PLAYING);

	INFO("playing audio for 5 seconds");
	Sleep(5000);

	GA_Pause();
	ASSERT("pause", GA_GetPlaybackState() == GA_PLAYBACK_STATE_PAUSED);
	INFO("pausing audio for 5 seconds");
	Sleep(5000);

	GA_SkipToTrack(1);
	ASSERT("skip to track 1", GA_GetCurrentTrackIndex() == 1);
	INFO("playing audio after skip for 5 seconds");
	Sleep(5000);

	GA_Pause();
	ASSERT("pause", GA_GetPlaybackState() == GA_PLAYBACK_STATE_PAUSED);
	INFO("pausing audio for 5 seconds");
	Sleep(5000);

	GA_Seek(155);
	ASSERT("seek", GA_GetTrackPosition() == 155);
	GA_Play();
	INFO("playing audio after seek for 15 seconds, no gaps should be heard");
	Sleep(15000);

	ASSERT("terminate", GA_Terminate() == GA_OK);
})

static char* all_tests() {
	RUN_TEST(basic);
	RUN_TEST(basic_playback);

	return 0;
}

int main() {
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

