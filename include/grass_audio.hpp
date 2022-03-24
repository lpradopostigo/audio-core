#pragma once
#include "bass.h"
#include "bassmix.h"
#include <functional>
#include <type_traits>
#include <iostream>
#include <string>
#include <utility>
#include "callable_to_pointer.hpp"
#include <array>
#include <map>

extern "C" {
#include "log.h"
}

#define NO_STREAM 0

enum GrassAudioPlaybackState {
  PLAYING = BASS_ACTIVE_PLAYING,
  STOPPED = BASS_ACTIVE_STOPPED,
  PAUSED = BASS_ACTIVE_PAUSED,
  STALLED = BASS_ACTIVE_STALLED,
  PAUSED_DEVICE = BASS_ACTIVE_PAUSED_DEVICE
};

enum GrassAudioEvent {
  POSITION_REACHED, // FIX
  END,
};

enum SampleRate {
  F44_1 = 44100,
  F48 = 48000
};

struct GrassAudioState {
  const GrassAudioPlaybackState playback_state{GrassAudioPlaybackState::STOPPED};
  const double file_position{0};
  const double file_duration{0};
  const int file_index{0};
};

class GrassAudio {
public:
	explicit GrassAudio(SampleRate sample_rate = SampleRate::F44_1);
	~GrassAudio();
	void set_files(std::vector<std::string> files);
	void play() const;
	void pause() const;
	void stop();
	void skip_to_index(int index);
	void next();
	void previous();
	void seek(double position) const;
	[[nodiscard]]  GrassAudioState get_state() const;
	void set_volume(float value) const;
	[[nodiscard]] DWORD add_listener(GrassAudioEvent event,
			const std::function<void()>& callback,
			bool remove_on_trigger = false, double position = 0) const;
	void remove_listener(DWORD listener) const;

	static void set_plugin_path(std::string plugin_path);
	static std::string get_plugin_path();
private:
	static std::string plugin_path_;
	std::vector<std::string> files_{};
	HSTREAM current_stream_{NO_STREAM};
	int current_file_index_{0};
	HSTREAM mixer_stream_{NO_STREAM};

	void load_next_file();
	void flush_mixer() const;
	int resolve_index(int index);
	static void log_bass_error(const std::string& message);
	static void load_bass_plugins();
};
