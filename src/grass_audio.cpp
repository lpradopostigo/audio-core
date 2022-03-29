#include "grass_audio.hpp"
#include <utility>

std::string GrassAudio::plugin_path_ = ".";

GrassAudio::GrassAudio(SampleRate sample_rate) {
	load_bass_plugins();

	BASS_Init(1, sample_rate, BASS_DEVICE_REINIT, nullptr, nullptr);
	log_bass_error("BASS_Init failed");

	BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 2000);
	log_bass_error("BASS_SetConfig failed");

	mixer_stream_ = BASS_Mixer_StreamCreate(sample_rate, 2, BASS_MIXER_END);
	log_bass_error("mixer stream creation failed");
}

GrassAudio::~GrassAudio() {
	BASS_StreamFree(mixer_stream_);
	log_bass_error("failed to free mixer stream");
}

void GrassAudio::play() const {
	if (current_stream_ == NO_HANDLER) { return; }
	BASS_ChannelPlay(mixer_stream_, FALSE);
	log_bass_error("failed to play the stream");
}

void GrassAudio::pause() const {
	if (current_stream_ == NO_HANDLER) { return; }
	BASS_ChannelPause(mixer_stream_);
	log_bass_error("failed to pause the stream");
}

void GrassAudio::stop() {
	remove_sync();

	if (current_stream_ != NO_HANDLER) {
		BASS_Mixer_ChannelRemove(current_stream_);
		log_bass_error("failed to remove the stream");
		current_stream_ = NO_HANDLER;
	}

	current_file_index_ = 0;
	load_next_file();
	set_sync();
}

void GrassAudio::skip_to_index(int index) {
	remove_sync();
	const auto playback_state =
			static_cast<const GrassAudioPlaybackState>(BASS_ChannelIsActive(mixer_stream_));
	BASS_Mixer_ChannelRemove(current_stream_);
	log_bass_error("could not remove the stream from the mixer");
	current_file_index_ = resolve_index(index);
	load_next_file();
	set_sync();

	if (playback_state == GrassAudioPlaybackState::PLAYING) {
		play();
	}
}

void GrassAudio::next() {
	skip_to_index(current_file_index_ + 1);

}

void GrassAudio::previous() {
	skip_to_index(current_file_index_ - 1);
}

void GrassAudio::seek(double position) const {
	BASS_Mixer_ChannelSetPosition(current_stream_,
			BASS_ChannelSeconds2Bytes(current_stream_, position),
			BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN | BASS_POS_MIXER_RESET);
	log_bass_error("failed to seek the stream");
}

GrassAudioState GrassAudio::get_state() const {
	const auto playback_state{
			static_cast<const GrassAudioPlaybackState>(BASS_ChannelIsActive(mixer_stream_))};
	const auto position_in_bytes{BASS_Mixer_ChannelGetPosition(current_stream_, BASS_POS_BYTE)};
	const auto length_in_bytes{BASS_ChannelGetLength(current_stream_, BASS_POS_BYTE)};
	const GrassAudioState state{
			.playback_state = playback_state,
			.file_position = BASS_ChannelBytes2Seconds(current_stream_, position_in_bytes),
			.file_duration = BASS_ChannelBytes2Seconds(current_stream_, length_in_bytes),
			.file_index = current_file_index_};

	return state;
}

void GrassAudio::set_volume(float value) const {
	BASS_ChannelSetAttribute(mixer_stream_, BASS_ATTRIB_VOL, value);
}

void GrassAudio::set_files(std::vector<std::string> files) {
	remove_sync();
	if (current_stream_ != NO_HANDLER) {
		BASS_Mixer_ChannelRemove(current_stream_);
		log_bass_error("failed to remove the stream");
	}

	files_ = std::move(files);
	current_file_index_ = 0;
	current_stream_ = NO_HANDLER;
	load_next_file();
	set_sync();
}

void GrassAudio::load_next_file() {
	const auto remaining_files{files_.size() - current_file_index_};
	if (remaining_files == 0) {
		return;
	}

	current_stream_ = BASS_StreamCreateFile(false,
			files_[current_file_index_].c_str(),
			0,
			0,
			BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);

	log_bass_error("failed to create stream");

	BASS_Mixer_StreamAddChannel(mixer_stream_,
			current_stream_,
			BASS_MIXER_CHAN_NORAMPIN | BASS_STREAM_AUTOFREE);
	log_bass_error("failed to add stream to mixer");

	BASS_ChannelSetPosition(mixer_stream_, 0, BASS_POS_BYTE);
	log_bass_error("failed to reset mixer position");
}

void GrassAudio::flush_mixer() const {
	if (current_stream_ != NO_HANDLER) {
		const auto channel_position{BASS_Mixer_ChannelGetPosition(current_stream_, BASS_POS_BYTE)};
		BASS_Mixer_ChannelSetPosition(current_stream_, channel_position, BASS_POS_BYTE);
	}
	BASS_ChannelSetPosition(mixer_stream_, 0, BASS_POS_BYTE);
}

int GrassAudio::resolve_index(int index) {
	const auto file_count{files_.size()};
	if (file_count > INT_MAX) {
		log_error("number of files out of limits");
		return INT_MAX;
	}

	if (index < 0) {
		return static_cast<int>(file_count) - 1;
	}
	else if (index >= file_count) {
		return 0;
	}
	else {
		return index;
	}
}

void GrassAudio::log_bass_error(const std::string& message) {
	const auto error_code{BASS_ErrorGetCode()};

	if (error_code != BASS_OK) {
		const auto log_message{message + " with BASS error code " + std::to_string(error_code)};
		log_error(log_message.c_str());
	}
}

void GrassAudio::load_bass_plugins() {
	log_info("plugin_path is %s", plugin_path_.c_str());
	BASS_PluginLoad(std::string{plugin_path_ + "/" + "bassflac.dll"}.c_str(), 0);
	log_bass_error("failed to load bassflac");
}

void GrassAudio::set_plugin_path(std::string plugin_path) {
	plugin_path_ = std::move(plugin_path);
}

std::string GrassAudio::get_plugin_path() {
	return plugin_path_;
}

void GrassAudio::set_sync() {
	if (sync_handler_ == NO_HANDLER) {
		sync_handler_ = BASS_ChannelSetSync(mixer_stream_,
				BASS_SYNC_END | BASS_SYNC_MIXTIME | BASS_SYNC_THREAD,
				0,
				handle_sync_,
				nullptr);
		log_bass_error("BASS_ChannelSetSync failed");
	}

}

void GrassAudio::remove_sync() {
	if (sync_handler_ != NO_HANDLER) {
		BASS_ChannelRemoveSync(mixer_stream_, sync_handler_);
		log_bass_error("BASS_ChannelRemoveSync failed");
		sync_handler_ = NO_HANDLER;
	}
}

