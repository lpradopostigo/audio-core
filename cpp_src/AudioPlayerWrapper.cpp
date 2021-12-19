#include "AudioPlayerWrapper.h"

Napi::Object AudioPlayerWrapper::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "AudioPlayerWrapper", {
      InstanceMethod("setFile", &AudioPlayerWrapper::SetFile),
      InstanceMethod("play", &AudioPlayerWrapper::Play),
      InstanceMethod("pause", &AudioPlayerWrapper::Pause),
      InstanceMethod("stop", &AudioPlayerWrapper::Stop),
      InstanceMethod("setPosition", &AudioPlayerWrapper::SetPosition),
      InstanceMethod("setVolume", &AudioPlayerWrapper::SetVolume),
      InstanceMethod("getPosition", &AudioPlayerWrapper::GetPosition),
  });

  auto *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);

  exports.Set("AudioPlayerWrapper", func);
  env.SetInstanceData<Napi::FunctionReference>(constructor);

  return exports;
}

AudioPlayerWrapper::AudioPlayerWrapper(const Napi::CallbackInfo &info) : ObjectWrap(info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "Number expected").ThrowAsJavaScriptException();
  }

  this->audioPlayer = new AudioPlayer();
}

void AudioPlayerWrapper::SetFile(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsString()) {
    Napi::TypeError::New(info.Env(), "string expected").ThrowAsJavaScriptException();
  }

  auto path = info[0].As<Napi::String>();
  this->audioPlayer->SetFile(path);
}
void AudioPlayerWrapper::Play(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "string expected").ThrowAsJavaScriptException();
  }

  this->audioPlayer->Play();
}

void AudioPlayerWrapper::Pause(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments")
        .ThrowAsJavaScriptException();
  }

  this->audioPlayer->Pause();
}

void AudioPlayerWrapper::Stop(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments").
        ThrowAsJavaScriptException();
  }

  this->audioPlayer->Stop();

}

void AudioPlayerWrapper::SetPosition(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "expected number").ThrowAsJavaScriptException();
  }

  const auto pos = info[0].As<Napi::Number>().DoubleValue();
  this->audioPlayer->SetPosition(pos);
}

void AudioPlayerWrapper::SetVolume(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "expected number").ThrowAsJavaScriptException();
  }

  const auto pos = info[0].As<Napi::Number>().FloatValue();
  this->audioPlayer->SetVolume(static_cast<int>(pos) >= 1 ? 1 : pos);
}

Napi::Value AudioPlayerWrapper::GetPosition(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments").
        ThrowAsJavaScriptException();
  }

  const auto pos = this->audioPlayer->GetPosition();
  return Napi::Number::New(info.Env(), pos);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  AudioPlayerWrapper::Init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)