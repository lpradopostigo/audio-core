#pragma once
#include "napi.h"
#include "AudioPlayer.h"

class AudioPlayerWrapper : public Napi::ObjectWrap<AudioPlayerWrapper> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  explicit AudioPlayerWrapper(const Napi::CallbackInfo &info);
 private:
  AudioPlayer *audioPlayer;

  void SetFile(const Napi::CallbackInfo &info);
  void Play(const Napi::CallbackInfo &info);
  void Pause(const Napi::CallbackInfo &info);
  void Stop(const Napi::CallbackInfo &info);
  void SetPosition(const Napi::CallbackInfo &info);
  void SetVolume(const Napi::CallbackInfo &info);
  Napi::Value GetPosition(const Napi::CallbackInfo &info);

};

