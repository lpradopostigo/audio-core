#include "AudioPlayerWrapper.h"

Napi::Object AudioPlayerWrapper::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "AudioPlayerWrapper", {
      InstanceMethod("setFile", &AudioPlayerWrapper::SetFile),
      InstanceMethod("play", &AudioPlayerWrapper::Play),
      InstanceMethod("pause", &AudioPlayerWrapper::Pause),
  });

  auto* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);

  exports.Set("AudioPlayerWrapper", func);
  env.SetInstanceData<Napi::FunctionReference>(constructor);

  return exports;
}

AudioPlayerWrapper::AudioPlayerWrapper(const Napi::CallbackInfo &info) : ObjectWrap(info) {
  Napi::Env env = info.Env();

  if (info.Length() != 0) {
    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
  }

  this->audioPlayer = new AudioPlayer();
}
void AudioPlayerWrapper::SetFile(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() != 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "string expected").ThrowAsJavaScriptException();
  }

  auto path = info[0].As<Napi::String>();
  this->audioPlayer->SetFile(path);

}
void AudioPlayerWrapper::Play(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() != 0) {
    Napi::TypeError::New(env, "string expected").ThrowAsJavaScriptException();
  }

  this->audioPlayer->Play();
}

void AudioPlayerWrapper::Pause(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (info.Length() != 0) {
    Napi::TypeError::New(env, "this method does not receive any arguments").ThrowAsJavaScriptException();
  }

  this->audioPlayer->Pause();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  AudioPlayerWrapper::Init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)