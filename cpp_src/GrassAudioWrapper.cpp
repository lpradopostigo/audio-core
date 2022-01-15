#include <iostream>
#include "GrassAudioWrapper.h"

Napi::Object GrassAudioWrapper::init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "GrassAudio", {
      InstanceMethod("play", &GrassAudioWrapper::play),
      InstanceMethod("pause", &GrassAudioWrapper::pause),
      InstanceMethod("stop", &GrassAudioWrapper::stop),
      InstanceMethod("setPosition", &GrassAudioWrapper::set_position),
      InstanceMethod("setVolume", &GrassAudioWrapper::set_volume),
      InstanceMethod("getPosition", &GrassAudioWrapper::get_position),
      InstanceMethod("on", &GrassAudioWrapper::on),
      InstanceMethod("once", &GrassAudioWrapper::once),
  });

  auto *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);

  exports.Set("GrassAudio", func);
  env.SetInstanceData<Napi::FunctionReference>(constructor);

  return exports;
}

GrassAudioWrapper::GrassAudioWrapper(const Napi::CallbackInfo &info) : ObjectWrap(info) {

  const auto files = info[0].As<Napi::Array>();

  const std::vector<std::vector<BYTE>> data{};

  for (int i = 0; i < files.Length(); ) {

  }

  std::vector files2{files[0].Data(), files[0].Data() + files[0].Length()};

  this->grass_audio_ = new GrassAudio(data);
}

void GrassAudioWrapper::play(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "string expected").ThrowAsJavaScriptException();
  }

  this->grass_audio_->play();
}

void GrassAudioWrapper::pause(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments")
        .ThrowAsJavaScriptException();
  }

  this->grass_audio_->pause();
}

void GrassAudioWrapper::stop(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments").
        ThrowAsJavaScriptException();
  }

  this->grass_audio_->stop();

}

void GrassAudioWrapper::set_position(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "expected number").ThrowAsJavaScriptException();
  }

  const auto position = info[0].As<Napi::Number>().DoubleValue();
  this->grass_audio_->set_position(position);
}

void GrassAudioWrapper::set_volume(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "expected number").ThrowAsJavaScriptException();
  }

  const auto position = info[0].As<Napi::Number>().FloatValue();
  this->grass_audio_->set_volume(static_cast<int>(position) >= 1 ? 1 : position);
}

Napi::Value GrassAudioWrapper::get_position(const Napi::CallbackInfo &info) {
  if (info.Length() != 0) {
    Napi::TypeError::New(info.Env(), "this method does not receive any arguments").
        ThrowAsJavaScriptException();
  }

  const auto position = this->grass_audio_->get_position();
  return Napi::Number::New(info.Env(), position);
}

void GrassAudioWrapper::set_file_from_memory(const Napi::CallbackInfo &info) {
  if (info.Length() != 1 || !info[0].IsBuffer()) {
    Napi::TypeError::New(info.Env(), "buffer expected").ThrowAsJavaScriptException();
  }
  auto path = info[0].As<Napi::Buffer<uint8_t>>();

  this->grass_audio_->set_file_from_memory(path.Data(), path.Length());
}

Napi::Value GrassAudioWrapper::on(const Napi::CallbackInfo &info) {
  const auto env = info.Env();
  const auto event_name = info[0].As<Napi::String>().Utf8Value();
  const auto callback = Napi::ThreadSafeFunction::New(env, info[1].As<Napi::Function>(), "", 0, 2);
  const auto callback_wrapper = [callback]() {
    callback.NonBlockingCall();
  };
  DWORD listener = 0;

  if (event_name == "positionSet") {
    listener = this->grass_audio_->on_position_set(callback_wrapper);
  } else if (event_name == "end") {
    listener = this->grass_audio_->on_end(callback_wrapper);
  } else if (event_name == "positionReached") {
    const auto position = info[2].As<Napi::Number>().DoubleValue();
    listener = this->grass_audio_->on_position_reached(callback_wrapper, position);
  }

  return Napi::Number::New(env, listener);
}

Napi::Value GrassAudioWrapper::once(const Napi::CallbackInfo &info) {
  const auto env = info.Env();
  const auto event_name = info[0].As<Napi::String>().Utf8Value();
  const auto callback = Napi::ThreadSafeFunction::New(env, info[1].As<Napi::Function>(), "", 0, 2);
  const auto callback_wrapper = [callback]() {
    callback.NonBlockingCall();
  };
  DWORD listener = 0;

  if (event_name == "positionSet") {
    listener = this->grass_audio_->on_position_set(callback_wrapper, true);
  } else if (event_name == "end") {
    listener = this->grass_audio_->on_end(callback_wrapper, true);
  } else if (event_name == "positionReached") {
    const auto position = info[2].As<Napi::Number>().DoubleValue();
    listener = this->grass_audio_->on_position_reached(callback_wrapper, position, true);
  }

  return Napi::Number::New(env, listener);
}

Napi::Value GrassAudioWrapper::get_length(const Napi::CallbackInfo &info) {
  const auto length = this->grass_audio_->get_length();
  return Napi::Number::New(info.Env(), length);
}

