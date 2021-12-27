#include "grass_audio_wrapper.h"

Napi::Object grass_audio_wrapper::init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "grass_audio", {
      InstanceMethod("setFile", &grass_audio_wrapper::set_file),
      InstanceMethod("setFileFromMemory", &grass_audio_wrapper::set_file_from_memory),
      InstanceMethod("play", &grass_audio_wrapper::play),
      InstanceMethod("pause", &grass_audio_wrapper::pause),
      InstanceMethod("stop", &grass_audio_wrapper::stop),
      InstanceMethod("setPosition", &grass_audio_wrapper::set_position),
      InstanceMethod("setVolume", &grass_audio_wrapper::set_volume),
      InstanceMethod("getPosition", &grass_audio_wrapper::get_position),
  });

  auto *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);

  exports.Set("grass_audio", func);
  env.SetInstanceData<Napi::FunctionReference>(constructor);

  return exports;
}

grass_audio_wrapper::grass_audio_wrapper(const Napi::CallbackInfo &info) : ObjectWrap(info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "Number expected").ThrowAsJavaScriptException();
  }

  this->audio_player = new grass_audio();
}

void grass_audio_wrapper::set_file(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsString()) {
    Napi::TypeError::New(info.Env(), "string expected").ThrowAsJavaScriptException();
  }

  auto path = info[0].As<Napi::String>();
  this->audio_player->set_file(static_cast<std::string>(path).c_str());
}
void grass_audio_wrapper::play(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "string expected").ThrowAsJavaScriptException();
  }

  this->audio_player->play();
}

void grass_audio_wrapper::pause(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments")
        .ThrowAsJavaScriptException();
  }

  this->audio_player->pause();
}

void grass_audio_wrapper::stop(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments").
        ThrowAsJavaScriptException();
  }

  this->audio_player->stop();

}

void grass_audio_wrapper::set_position(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "expected number").ThrowAsJavaScriptException();
  }

  const auto position = info[0].As<Napi::Number>().DoubleValue();
  this->audio_player->set_position(position);
}

void grass_audio_wrapper::set_volume(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "expected number").ThrowAsJavaScriptException();
  }

  const auto position = info[0].As<Napi::Number>().FloatValue();
  this->audio_player->set_volume(static_cast<int>(position) >= 1 ? 1 : position);
}

Napi::Value grass_audio_wrapper::get_position(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments").
        ThrowAsJavaScriptException();
  }

  const auto position = this->audio_player->get_position();
  return Napi::Number::New(info.Env(), position);
}

void grass_audio_wrapper::set_file_from_memory(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsBuffer()) {
    Napi::TypeError::New(info.Env(), "buffer expected").ThrowAsJavaScriptException();
  }
  auto path = info[0].As<Napi::Buffer<uint8_t>>();

  this->audio_player->set_file_from_memory(path.Data(), path.Length());
}
