#include <iostream>
#include "grass_audio_wrapper.h"

Napi::Object grass_audio_wrapper::init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "GrassAudio", {
      InstanceMethod("setFile", &grass_audio_wrapper::set_file),
      InstanceMethod("setFileFromMemory", &grass_audio_wrapper::set_file_from_memory),
      InstanceMethod("play", &grass_audio_wrapper::play),
      InstanceMethod("pause", &grass_audio_wrapper::pause),
      InstanceMethod("stop", &grass_audio_wrapper::stop),
      InstanceMethod("setPosition", &grass_audio_wrapper::set_position),
      InstanceMethod("setVolume", &grass_audio_wrapper::set_volume),
      InstanceMethod("getPosition", &grass_audio_wrapper::get_position),
      InstanceMethod("on", &grass_audio_wrapper::on),
      InstanceMethod("once", &grass_audio_wrapper::once),
      InstanceMethod("getLength", &grass_audio_wrapper::get_length),
  });

  auto *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);

  exports.Set("GrassAudio", func);
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

Napi::Value grass_audio_wrapper::on(const Napi::CallbackInfo &info) {
  const auto env = info.Env();
  const auto event_name = info[0].As<Napi::String>().Utf8Value();
  const auto callback = Napi::ThreadSafeFunction::New(env, info[1].As<Napi::Function>(), "", 0, 2);
  const auto callback_wrapper = [callback]() {
    callback.NonBlockingCall();
  };
  DWORD listener = 0;

  if (event_name == "positionSet") {
    listener = this->audio_player->on_position_set(callback_wrapper);
  } else if (event_name == "end") {
    listener = this->audio_player->on_end(callback_wrapper);
  } else if (event_name == "positionReached") {
    const auto position = info[2].As<Napi::Number>().DoubleValue();
    listener = this->audio_player->on_position_reached(callback_wrapper, position);
  }

  return Napi::Number::New(env, listener);
}

Napi::Value grass_audio_wrapper::once(const Napi::CallbackInfo &info) {
  const auto env = info.Env();
  const auto event_name = info[0].As<Napi::String>().Utf8Value();
  const auto callback = Napi::ThreadSafeFunction::New(env, info[1].As<Napi::Function>(), "", 0, 2);
  const auto callback_wrapper = [callback]() {
    callback.NonBlockingCall();
  };
  DWORD listener = 0;

  if (event_name == "positionSet") {
    listener = this->audio_player->on_position_set(callback_wrapper, true);
  } else if (event_name == "end") {
    listener = this->audio_player->on_end(callback_wrapper, true);
  } else if (event_name == "positionReached") {
    const auto position = info[2].As<Napi::Number>().DoubleValue();
    listener = this->audio_player->on_position_reached(callback_wrapper, position, true);
  }

  return Napi::Number::New(env, listener);
}

Napi::Value grass_audio_wrapper::get_length(const Napi::CallbackInfo &info) {
  const auto length = this->audio_player->get_length();
  return Napi::Number::New(info.Env(), length);
}

