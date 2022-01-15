#pragma once
#include "napi.h"
#include "GrassAudio.h"

class GrassAudioWrapper : public Napi::ObjectWrap<GrassAudioWrapper> {
public:
  static Napi::Object init(Napi::Env env, Napi::Object exports);
  explicit GrassAudioWrapper(const Napi::CallbackInfo &info);
private:
  GrassAudio *grass_audio_;
  void play(const Napi::CallbackInfo &info);
  void pause(const Napi::CallbackInfo &info);
  void stop(const Napi::CallbackInfo &info);
  void set_position(const Napi::CallbackInfo &info);
  void set_volume(const Napi::CallbackInfo &info);
  Napi::Value get_position(const Napi::CallbackInfo &info);
  Napi::Value on(const Napi::CallbackInfo &info);
  Napi::Value once(const Napi::CallbackInfo &info);
};

