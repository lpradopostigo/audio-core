#pragma once
#include "napi.h"
#include "AudioPlayer.h"

class AudioPlayerWrapper : public Napi::ObjectWrap<AudioPlayerWrapper> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  explicit AudioPlayerWrapper(const Napi::CallbackInfo &info);
 private:
  void SetFile(const Napi::CallbackInfo &info);
  void Play(const Napi::CallbackInfo &info);
  void Pause(const Napi::CallbackInfo &info);
  AudioPlayer *audioPlayer;
};

