#pragma once
#include "bass.h"
#include "bassmix.h"
#include <functional>
#include <type_traits>
#include <iostream>
#include <string>
#include <utility>
#include "callable_to_pointer.hpp"

extern "C" {
#include "log.h"
}

template<typename T>
concept BinaryOrFilePath = std::is_same_v<T, std::string>
		|| std::is_same_v<T, std::vector<uint8_t>>;

enum GrassAudioEvent {
  POSITION_REACHED, // FIX
  END,
};

template<BinaryOrFilePath T = std::string>
class [[maybe_unused]] GrassAudio {
public:
	[[maybe_unused]] explicit GrassAudio(std::vector<T> files, DWORD frequency = 44100) {
		files_ = std::move(files);
		init(frequency);
		load_next_file();
	}

	explicit GrassAudio(DWORD frequency = 44100) {
		init(frequency);
	}

	~GrassAudio() {
		pause();
		BASS_StreamFree(mixer_stream_);
		log_bass_error("failed to free mixer stream");
	}

	[[maybe_unused]] void play() const {
		if (current_stream_ != 0) {
			BASS_ChannelPlay(mixer_stream_, FALSE);
			log_bass_error("failed to play");
		}
	}

	[[maybe_unused]] void pause() const {
		BASS_ChannelPause(mixer_stream_);
	}

	[[maybe_unused]] void stop() {
		pause();
		flush_mixer();
		BASS_Mixer_ChannelRemove(current_stream_);

		current_file_index_ = 0;
		load_next_file();
	}

	[[maybe_unused]] void set_position(double position) const {
		BASS_Mixer_ChannelSetPosition(current_stream_,
				BASS_ChannelSeconds2Bytes(current_stream_, position),
				BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN | BASS_POS_MIXER_RESET);

		log_bass_error("failed to set position");
	};

	[[maybe_unused]] [[nodiscard]]  double get_position() const {
		const QWORD position_in_bytes = BASS_Mixer_ChannelGetPosition(current_stream_, BASS_POS_BYTE);
		return BASS_ChannelBytes2Seconds(current_stream_, position_in_bytes);
	};

	[[maybe_unused]] void set_volume(float value) const {
		BASS_ChannelSetAttribute(mixer_stream_, BASS_ATTRIB_VOL, value);

	};

	[[maybe_unused]] void skip_to_index(int index) {
		BASS_Mixer_ChannelRemove(current_stream_);
		current_file_index_ = resolve_index(index);
		load_next_file();
	}

	[[maybe_unused]] void next() {
		skip_to_index(current_file_index_ + 1);

	}

	[[maybe_unused]] void previous() {
		skip_to_index(current_file_index_ - 1);
	}

	[[maybe_unused]] [[nodiscard]] DWORD add_listener(GrassAudioEvent event,
			const std::function<void()>& callback,
			bool remove_on_trigger = false, double position = 0) const {
		const auto c_callback = callable_to_pointer([callback](HSYNC, DWORD, DWORD, void*) { callback(); });
		const DWORD one_time = remove_on_trigger ? BASS_SYNC_ONETIME : 0;

		DWORD listener = 0;

		switch (event) {
		case POSITION_REACHED: {
			const QWORD position_in_bytes = BASS_ChannelSeconds2Bytes(mixer_stream_, position);
			listener = BASS_ChannelSetSync(mixer_stream_, BASS_SYNC_POS | BASS_SYNC_MIXTIME | one_time,
					position_in_bytes,
					c_callback, nullptr);
			break;
		}

		case END: {
			listener = BASS_ChannelSetSync(mixer_stream_, BASS_SYNC_END | BASS_SYNC_MIXTIME | one_time,
					0,
					c_callback, nullptr);
			break;
		}

		default:break;
		}

		log_bass_error("failed to set end_sync");
		return listener;
	}

	[[maybe_unused]] [[nodiscard]] size_t get_current_file_index() const {
		return current_file_index_;

	};

	[[maybe_unused]] void remove_listener(DWORD listener) const {
		BASS_ChannelRemoveSync(mixer_stream_, listener);
		log_bass_error("failed to remove end_sync");
	};

	[[maybe_unused]] void set_files(std::vector<T> files) {
		pause();
		flush_mixer();
		if (current_stream_ != 0)
			BASS_Mixer_ChannelRemove(current_stream_);
		files_ = std::move(files);
		current_file_index_ = 0;
		current_stream_ = 0;

		load_next_file();
	}

	[[maybe_unused]] [[nodiscard]] double get_length() const {
		const auto length_in_bytes = BASS_ChannelGetLength(current_stream_, BASS_POS_BYTE);
		if (BASS_ErrorGetCode() != BASS_OK) {
			return 0;
		}

		const auto length_in_seconds = BASS_ChannelBytes2Seconds(current_stream_, length_in_bytes);
		log_bass_error("failed to convert length byte to seconds");
		return length_in_seconds;
	}

	[[maybe_unused]] [[nodiscard]] DWORD get_status() const {
		return BASS_ChannelIsActive(mixer_stream_);
	}

private:
	std::vector<T> files_{};
	DWORD current_stream_ = 0;
	size_t current_file_index_ = 0;
	HSTREAM mixer_stream_ = 0;

	void load_next_file() {
		const auto remaining_files = files_.size() - current_file_index_;
		if (remaining_files == 0) {
			log_info("playlist ended");
			return;
		}

		if constexpr(std::is_same_v<T, std::string>) {
			current_stream_ = BASS_StreamCreateFile(false,
					files_[current_file_index_].c_str(),
					0,
					0,
					BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);

		}
		else if (std::is_same_v<T, std::vector<uint8_t>>) {
			current_stream_ = BASS_StreamCreateFile(true,
					files_[current_file_index_].data(),
					0,
					files_[current_file_index_].size(),
					BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
		}
		log_bass_error("failed to create stream");

		BASS_Mixer_StreamAddChannel(mixer_stream_,
				current_stream_,
				BASS_MIXER_CHAN_NORAMPIN | BASS_STREAM_AUTOFREE);
		log_bass_error("failed to add stream to mixer");

		BASS_ChannelSetPosition(mixer_stream_, 0, BASS_POS_BYTE);
		log_bass_error("failed to reset mixer position");
	}

	void flush_mixer() const {
		if (current_stream_ != 0) {
			const auto channel_position = BASS_Mixer_ChannelGetPosition(current_stream_, BASS_POS_BYTE);
			BASS_Mixer_ChannelSetPosition(current_stream_, channel_position, BASS_POS_BYTE);
		}
		BASS_ChannelSetPosition(mixer_stream_, 0, BASS_POS_BYTE);
	}

	size_t resolve_index(int index) {
		if (index < 0) {
			return files_.size() - 1;
		}
		else if (index >= files_.size()) {
			return 0;
		}
		else {
			return index;
		}
	}

	static void log_bass_error(const std::string& message) {
		const auto error_code = BASS_ErrorGetCode();

		if (error_code != BASS_OK) {
			const auto log_message = message + " with BASS error code " + std::to_string(error_code);
			log_error(log_message.c_str());
		}
	}

	void init(DWORD frequency) {
		BASS_Init(1, frequency, BASS_DEVICE_REINIT, nullptr, nullptr);
		log_bass_error("BASS_Init failed");

		BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 2000);
		log_bass_error("BASS_SetConfig failed");

		mixer_stream_ = BASS_Mixer_StreamCreate(frequency, 2, BASS_MIXER_END);
		log_bass_error("mixer stream creation failed");

		const auto c_callback = callable_to_pointer([this](HSYNC, DWORD, DWORD, void*) -> void {
		  current_file_index_++;
		  load_next_file();
		});

		BASS_ChannelSetSync(mixer_stream_,
				BASS_SYNC_END | BASS_SYNC_MIXTIME,
				0,
				reinterpret_cast<SYNCPROC (__stdcall*)>(c_callback),
				nullptr);
		log_bass_error("BASS_ChannelSetSync failed");
	}
};
