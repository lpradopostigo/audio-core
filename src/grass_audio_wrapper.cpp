#include <iostream>
#include "grass_audio_wrapper.hpp"

Napi::Object GrassAudioWrapper::init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "GrassAudio", {
			InstanceMethod("play", &GrassAudioWrapper::play),
			InstanceMethod("pause", &GrassAudioWrapper::pause),
			InstanceMethod("stop", &GrassAudioWrapper::stop),
			InstanceMethod("skipToIndex", &GrassAudioWrapper::skip_to_index),
			InstanceMethod("next", &GrassAudioWrapper::next),
			InstanceMethod("previous", &GrassAudioWrapper::previous),
			InstanceMethod("seek", &GrassAudioWrapper::seek),
			InstanceMethod("setFiles", &GrassAudioWrapper::set_files),
			InstanceMethod("setVolume", &GrassAudioWrapper::set_volume),
			InstanceMethod("getState", &GrassAudioWrapper::get_state),
			StaticMethod("setupEnvironment", &GrassAudioWrapper::setup_environment)
	});
	auto* constructor = new Napi::FunctionReference();
	*constructor = Napi::Persistent(func);
	exports.Set("GrassAudio", func);
	env.SetInstanceData<Napi::FunctionReference>(constructor);

	return exports;
}

GrassAudioWrapper::GrassAudioWrapper(const Napi::CallbackInfo& info)
		:ObjectWrap(info) {
	if (info.Length() == 1) {
		const auto sample_rate = info[0].As<Napi::Number>().Int32Value();
		this->grass_audio_ = new GrassAudio(static_cast<SampleRate>(sample_rate));
	}
	else {
		this->grass_audio_ = new GrassAudio();
	}

}

GrassAudioWrapper::~GrassAudioWrapper() {
	delete this->grass_audio_;
}

void GrassAudioWrapper::play(const Napi::CallbackInfo& info) {
	this->grass_audio_->play();
}

void GrassAudioWrapper::pause(const Napi::CallbackInfo& info) {
	this->grass_audio_->pause();
}

void GrassAudioWrapper::stop(const Napi::CallbackInfo& info) {
	this->grass_audio_->stop();
}

void GrassAudioWrapper::seek(const Napi::CallbackInfo& info) {
	const auto position = info[0].As<Napi::Number>().DoubleValue();
	this->grass_audio_->seek(position);
}

void GrassAudioWrapper::skip_to_index(const Napi::CallbackInfo& info) {
	const auto index = info[0].As<Napi::Number>().Int32Value();
	this->grass_audio_->skip_to_index(index);
}

void GrassAudioWrapper::next(const Napi::CallbackInfo& info) {
	this->grass_audio_->next();
}

void GrassAudioWrapper::previous(const Napi::CallbackInfo& info) {
	this->grass_audio_->previous();
}

// experimental
void GrassAudioWrapper::set_volume(const Napi::CallbackInfo& info) {
	const auto position = info[0].As<Napi::Number>().FloatValue();
	this->grass_audio_->set_volume(static_cast<int>(position) >= 1 ? 1 : position);
}

void GrassAudioWrapper::set_files(const Napi::CallbackInfo& info) {
	const auto file_paths = info[0].As<Napi::Array>();

	std::vector<std::string> file_paths_native;
	file_paths_native.resize(file_paths.Length());

	for (uint32_t i = 0; i < file_paths.Length(); i++) {
		file_paths_native[i] = file_paths[i].As<Napi::String>().Utf8Value();
	}

	this->grass_audio_->set_files(file_paths_native);
}

Napi::Value GrassAudioWrapper::get_state(const Napi::CallbackInfo& info) {
	const auto env = info.Env();
	const auto state = this->grass_audio_->get_state();
	auto js_state = Napi::Object::New(env);

	std::string js_playback_state{};

	switch (state.playback_state) {
	case GrassAudioPlaybackState::PLAYING: {
		js_playback_state = "playing";
		break;
	}

	case GrassAudioPlaybackState::PAUSED: {
		js_playback_state = "paused";
		break;
	}

	default:
	case GrassAudioPlaybackState::STOPPED: {
		js_playback_state = "stopped";
		break;
	}
	}

	js_state.Set("playbackState", js_playback_state);
	js_state.Set("fileDuration", state.file_duration);
	js_state.Set("filePosition", state.file_position);
	js_state.Set("fileIndex", state.file_index);

	return js_state;

}

void GrassAudioWrapper::setup_environment(const Napi::CallbackInfo& info) {
	const auto plugin_path = info[0].As<Napi::String>().Utf8Value();
	GrassAudio::set_plugin_path(plugin_path);
}



