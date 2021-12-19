#pragma once
#include "napi.h"
#include "GrassAudio.h"

class GrassAudioWrapper : public Napi::ObjectWrap<GrassAudioWrapper> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  explicit GrassAudioWrapper(const Napi::CallbackInfo &info);
 private:
  GrassAudio *audioPlayer;

  void SetFile(const Napi::CallbackInfo &info);
  void Play(const Napi::CallbackInfo &info);
  void Pause(const Napi::CallbackInfo &info);
  void Stop(const Napi::CallbackInfo &info);
  void SetPosition(const Napi::CallbackInfo &info);
  void SetVolume(const Napi::CallbackInfo &info);
  Napi::Value GetPosition(const Napi::CallbackInfo &info);

};

