#pragma once
#include "napi.h"
#include "GrassAudio.h"

class GrassAudioWrapper : public Napi::ObjectWrap<GrassAudioWrapper> {
public:
  static Napi::Object init(Napi::Env env, Napi::Object exports);
  explicit GrassAudioWrapper(const Napi::CallbackInfo &info);
private:
  GrassAudio<std::string> *grass_audio_;

  void play(const Napi::CallbackInfo &info);
  void pause(const Napi::CallbackInfo &info);
  void stop(const Napi::CallbackInfo &info);
  void next(const Napi::CallbackInfo &info);
  void previous(const Napi::CallbackInfo &info);
  void skip_to_file(const Napi::CallbackInfo &info);
  Napi::Value get_current_file_index(const Napi::CallbackInfo &info);
  void set_position(const Napi::CallbackInfo &info);
  void set_volume(const Napi::CallbackInfo &info);
  Napi::Value get_position(const Napi::CallbackInfo &info);
  Napi::Value add_listener(const Napi::CallbackInfo &info);
  void remove_listener(const Napi::CallbackInfo &info);
};

