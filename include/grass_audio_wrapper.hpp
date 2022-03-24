#pragma once
#include "napi.h"
#include "grass_audio.hpp"

class GrassAudioWrapper : public Napi::ObjectWrap<GrassAudioWrapper> {
public:
	static Napi::Object init(Napi::Env env, Napi::Object exports);
	explicit GrassAudioWrapper(const Napi::CallbackInfo& info);
	~GrassAudioWrapper() override;

private:
	GrassAudio* grass_audio_;

	void play(const Napi::CallbackInfo& info);
	void pause(const Napi::CallbackInfo& info);
	void stop(const Napi::CallbackInfo& info);
	void next(const Napi::CallbackInfo& info);
	void previous(const Napi::CallbackInfo& info);
	void skip_to_index(const Napi::CallbackInfo& info);
	void set_volume(const Napi::CallbackInfo& info);
	Napi::Value add_listener(const Napi::CallbackInfo& info);
	void remove_listener(const Napi::CallbackInfo& info);
	void set_files(const Napi::CallbackInfo& info);
	Napi::Value get_state(const Napi::CallbackInfo& info);
	void seek(const Napi::CallbackInfo& info);
	static void setup_environment(const Napi::CallbackInfo& info);
};

