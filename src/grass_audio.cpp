#include "grass_audio.hpp"

GrassAudio::GrassAudio(SampleRate sample_rate) {
	load_bass_plugins();

	BASS_Init(1, sample_rate, BASS_DEVICE_REINIT, nullptr, nullptr);
	log_bass_error("BASS_Init failed");

	BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 2000);
	log_bass_error("BASS_SetConfig failed");

	mixer_stream_ = BASS_Mixer_StreamCreate(sample_rate, 2, BASS_MIXER_END);
	log_bass_error("mixer stream creation failed");

	const auto c_callback{callable_to_pointer([this](HSYNC, DWORD, DWORD, void*) -> void {
	  current_file_index_++;
	  load_next_file();
	})};

	BASS_ChannelSetSync(mixer_stream_,
			BASS_SYNC_END | BASS_SYNC_MIXTIME,
			0,
			reinterpret_cast<SYNCPROC (__stdcall*)>(c_callback),
			nullptr);
	log_bass_error("BASS_ChannelSetSync failed");
}

GrassAudio::~GrassAudio() {
	BASS_StreamFree(mixer_stream_);
	log_bass_error("failed to free mixer stream");
}

void GrassAudio::play() const {
	if (current_stream_ == NO_STREAM) { return; }
	BASS_ChannelPlay(mixer_stream_, FALSE);
	log_bass_error("failed to play the stream");
}

void GrassAudio::pause() const {
	BASS_ChannelPause(mixer_stream_);
	log_bass_error("failed to pause the stream");
}

void GrassAudio::stop() {
	pause();
	flush_mixer();
	BASS_Mixer_ChannelRemove(current_stream_);
	log_bass_error("failed to remove the stream");
	current_file_index_ = 0;
	load_next_file();
}

void GrassAudio::skip_to_index(int index) {
	BASS_Mixer_ChannelRemove(current_stream_);
	current_file_index_ = resolve_index(index);
	load_next_file();
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

DWORD GrassAudio::add_listener(GrassAudioEvent event, const std::function<void()>& callback, bool remove_on_trigger,
		double position) const {
	const auto c_callback{
			callable_to_pointer([callback](HSYNC, DWORD, DWORD, void*) { callback(); })};

	const auto one_time{remove_on_trigger ? BASS_SYNC_ONETIME : 0};

	DWORD listener{0};

	switch (event) {
	case POSITION_REACHED: {
		const auto position_in_bytes{BASS_ChannelSeconds2Bytes(mixer_stream_, position)};
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

void GrassAudio::remove_listener(DWORD listener) const {
	BASS_ChannelRemoveSync(mixer_stream_, listener);
	log_bass_error("failed to remove end_sync");
}

void GrassAudio::set_files(std::vector<std::string> files) {
	if (current_stream_ != NO_STREAM) {
		pause();
		flush_mixer();
		BASS_Mixer_ChannelRemove(current_stream_);
	}

	files_ = std::move(files);
	current_file_index_ = 0;
	current_stream_ = NO_STREAM;

	load_next_file();
}

void GrassAudio::load_next_file() {
	const auto remaining_files{files_.size() - current_file_index_};
	if (remaining_files == 0) {
		log_info("playlist ended");
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
	if (current_stream_ != NO_STREAM) {
		const auto channel_position{BASS_Mixer_ChannelGetPosition(current_stream_, BASS_POS_BYTE)};
		BASS_Mixer_ChannelSetPosition(current_stream_, channel_position, BASS_POS_BYTE);
	}
	BASS_ChannelSetPosition(mixer_stream_, 0, BASS_POS_BYTE);
}

int GrassAudio::resolve_index(int index) {
	const auto file_count{files_.size()};
	if (file_count > INT_MAX) {
		log_error("number of files out limits");
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
	BASS_PluginLoad("bassflac.dll", 0);
	log_bass_error("failed to load bassflac");
}
