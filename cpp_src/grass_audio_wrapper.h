#pragma once
#include "napi.h"
#include "grass_audio.h"

class grass_audio_wrapper : public Napi::ObjectWrap<grass_audio_wrapper> {
 public:
  static Napi::Object init(Napi::Env env, Napi::Object exports);
  explicit grass_audio_wrapper(const Napi::CallbackInfo &info);
 private:
  grass_audio *audio_player;

  void set_file(const Napi::CallbackInfo &info);
  void set_file_from_memory(const Napi::CallbackInfo &info);
  void play(const Napi::CallbackInfo &info);
  void pause(const Napi::CallbackInfo &info);
  void stop(const Napi::CallbackInfo &info);
  void set_position(const Napi::CallbackInfo &info);
  void set_volume(const Napi::CallbackInfo &info);
  Napi::Value get_position(const Napi::CallbackInfo &info);
  Napi::Value on(const Napi::CallbackInfo &info);

};

