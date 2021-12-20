#include "GrassAudioWrapper.h"

Napi::Object GrassAudioWrapper::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "GrassAudio", {
      InstanceMethod("setFile", &GrassAudioWrapper::SetFile),
      InstanceMethod("setFileFromMemory", &GrassAudioWrapper::SetFileFromMemory),
      InstanceMethod("play", &GrassAudioWrapper::Play),
      InstanceMethod("pause", &GrassAudioWrapper::Pause),
      InstanceMethod("stop", &GrassAudioWrapper::Stop),
      InstanceMethod("setPosition", &GrassAudioWrapper::SetPosition),
      InstanceMethod("setVolume", &GrassAudioWrapper::SetVolume),
      InstanceMethod("getPosition", &GrassAudioWrapper::GetPosition),
  });

  auto *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);

  exports.Set("GrassAudio", func);
  env.SetInstanceData<Napi::FunctionReference>(constructor);

  return exports;
}

GrassAudioWrapper::GrassAudioWrapper(const Napi::CallbackInfo &info) : ObjectWrap(info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "Number expected").ThrowAsJavaScriptException();
  }

  this->audioPlayer = new GrassAudio();
}

void GrassAudioWrapper::SetFile(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsString()) {
    Napi::TypeError::New(info.Env(), "string expected").ThrowAsJavaScriptException();
  }

  auto path = info[0].As<Napi::String>();
  this->audioPlayer->SetFile(path);
}
void GrassAudioWrapper::Play(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "string expected").ThrowAsJavaScriptException();
  }

  this->audioPlayer->Play();
}

void GrassAudioWrapper::Pause(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments")
        .ThrowAsJavaScriptException();
  }

  this->audioPlayer->Pause();
}

void GrassAudioWrapper::Stop(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments").
        ThrowAsJavaScriptException();
  }

  this->audioPlayer->Stop();

}

void GrassAudioWrapper::SetPosition(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "expected number").ThrowAsJavaScriptException();
  }

  const auto pos = info[0].As<Napi::Number>().DoubleValue();
  this->audioPlayer->SetPosition(pos);
}

void GrassAudioWrapper::SetVolume(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "expected number").ThrowAsJavaScriptException();
  }

  const auto pos = info[0].As<Napi::Number>().FloatValue();
  this->audioPlayer->SetVolume(static_cast<int>(pos) >= 1 ? 1 : pos);
}

Napi::Value GrassAudioWrapper::GetPosition(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments").
        ThrowAsJavaScriptException();
  }

  const auto pos = this->audioPlayer->GetPosition();
  return Napi::Number::New(info.Env(), pos);
}

void GrassAudioWrapper::SetFileFromMemory(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsBuffer()) {
    Napi::TypeError::New(info.Env(), "buffer expected").ThrowAsJavaScriptException();
  }
  auto path = info[0].As<Napi::Buffer<uint8_t>>();

  this->audioPlayer->SetFileFromMemory(path.Data(), path.Length());
}
