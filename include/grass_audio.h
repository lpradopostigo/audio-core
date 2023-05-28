#pragma once
#include <stdint.h>

enum GAResult {
  GA_RESULT_OK = 0,
  GA_RESULT_ERROR = 1
};

enum GAPlaybackState {
  GA_PLAYBACK_STATE_STOPPED = 0,
  GA_PLAYBACK_STATE_PLAYING = 1,
  GA_PLAYBACK_STATE_PAUSED = 2
};

/**
 * Initializes the audio system.
 * @param sample_rate The sample rate of the audio system.
 * @return GA_RESULT_OK if the audio system was initialized successfully, GA_RESULT_ERROR otherwise.
 */
enum GAResult ga_init(uint32_t sample_rate);

/**
 * Terminates the audio system.
 * @return GA_RESULT_OK if the audio system was terminated successfully, GA_RESULT_ERROR otherwise.
 */
enum GAResult ga_terminate(void);

/**
 * Sets the playlist.
 * @param playlist The playlist.
 * @param playlist_size The size of the playlist.
 */
void ga_set_playlist(char const* const* playlist, uint16_t playlist_size);

/**
 * Plays the current audio file if none, plays the first audio file in the playlist.
 */
void ga_play(void);

/**
 * Pauses the current audio file.
 */
void ga_pause(void);

/**
 * Stops the current audio file, and set the playlist index to 0.
 */
void ga_stop(void);

/**
 * Set the playlist index to the previous audio file, if the current audio file is the first audio file, set the playlist index to the last audio file.
 */
void ga_previous(void);

/**
 * Set the playlist index to the next audio file, if the current audio file is the last audio file, set the playlist index to the first audio file.
 */
void ga_next(void);

/**
 * Sets the volume.
 * @param volume values between 0 and 1 are normal, values greater than 1 will amplify the sound.
 */
void ga_set_volume(float volume);

float ga_get_volume(void);
void ga_seek(double position);

/**
 * Sets the playlist index, if the index is less than 0, set the playlist index to 0, if the index is greater than the playlist size, set the playlist index to the last audio file.
 * @param index The playlist index.
 */
void ga_skip_to(int16_t index);

uint16_t ga_get_playlist_index(void);

/**
 * Gets the current audio file path, will allocate memory for the path, the caller is responsible for freeing the memory.
 * @return The current audio file path.
 */
const char* ga_get_playlist_path(void);

uint16_t ga_get_playlist_size(void);
enum GAPlaybackState ga_get_playback_state(void);

/**
 * Gets the current audio file position.
 * @return The current audio file position, if none, returns 0.
 */
double ga_get_position(void);

/**
 * Gets the current audio file length.
 * @return The current audio file length, if none, returns 0.
 */
double ga_get_length(void);
