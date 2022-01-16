#include <iostream>
#include "GrassAudioWrapper.h"

Napi::Object GrassAudioWrapper::init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "GrassAudio", {
      InstanceMethod("play", &GrassAudioWrapper::play),
      InstanceMethod("pause", &GrassAudioWrapper::pause),
      InstanceMethod("stop", &GrassAudioWrapper::stop),
      InstanceMethod("next", &GrassAudioWrapper::next),
      InstanceMethod("previous", &GrassAudioWrapper::previous),
      InstanceMethod("skipToFile", &GrassAudioWrapper::skip_to_file),
      InstanceMethod("getCurrentFileIndex", &GrassAudioWrapper::get_current_file_index),
      InstanceMethod("setPosition", &GrassAudioWrapper::set_position),
      InstanceMethod("setVolume", &GrassAudioWrapper::set_volume),
      InstanceMethod("getPosition", &GrassAudioWrapper::get_position),
      InstanceMethod("addListener", &GrassAudioWrapper::add_listener),
      InstanceMethod("removeListener", &GrassAudioWrapper::remove_listener),
  });

  auto *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);

  exports.Set("GrassAudio", func);
  env.SetInstanceData<Napi::FunctionReference>(constructor);

  return exports;
}

GrassAudioWrapper::GrassAudioWrapper(const Napi::CallbackInfo &info) : ObjectWrap(info) {
  const auto file_paths = info[0].As<Napi::Array>();

  std::vector<std::string> file_paths_native;
  file_paths_native.resize(file_paths.Length());

  for (uint32_t i = 0; i < file_paths.Length(); i++) {
    file_paths_native[i] = file_paths[i].As<Napi::String>().Utf8Value();
  }

  this->grass_audio_ = new GrassAudio(file_paths_native);
}

void GrassAudioWrapper::play(const Napi::CallbackInfo &info) {
  this->grass_audio_->play();
}

void GrassAudioWrapper::pause(const Napi::CallbackInfo &info) {
  this->grass_audio_->pause();
}

void GrassAudioWrapper::stop(const Napi::CallbackInfo &info) {
  this->grass_audio_->stop();
}

void GrassAudioWrapper::set_position(const Napi::CallbackInfo &info) {
  const auto position = info[0].As<Napi::Number>().DoubleValue();
  this->grass_audio_->set_position(position);
}

void GrassAudioWrapper::set_volume(const Napi::CallbackInfo &info) {
  const auto position = info[0].As<Napi::Number>().FloatValue();
  this->grass_audio_->set_volume(static_cast<int>(position) >= 1 ? 1 : position);
}

Napi::Value GrassAudioWrapper::get_position(const Napi::CallbackInfo &info) {
  const auto position = this->grass_audio_->get_position();
  return Napi::Number::New(info.Env(), position);
}

Napi::Value GrassAudioWrapper::add_listener(const Napi::CallbackInfo &info) {
  const auto env = info.Env();
  const auto event_name = info[0].As<Napi::String>().Utf8Value();
  const auto callback = Napi::ThreadSafeFunction::New(env, info[1].As<Napi::Function>(), "", 0, 2);
  const auto remove_on_trigger = info[2].As<Napi::Boolean>().Value();

  const auto callback_wrapper = [callback]() {
    callback.NonBlockingCall();
  };

  DWORD listener = 0;

  if (event_name == "end") {
    listener = this->grass_audio_->add_listener(GrassAudioEvent::END, callback_wrapper, remove_on_trigger);
  } else if (event_name == "positionReached") {
    const auto position = info[2].As<Napi::Number>().DoubleValue();
    listener = this->grass_audio_->add_listener(GrassAudioEvent::POSITION_REACHED,
                                                callback_wrapper,
                                                remove_on_trigger,
                                                position);
  }

  return Napi::Number::New(env, listener);
}

void GrassAudioWrapper::remove_listener(const Napi::CallbackInfo &info) {
  const auto listener = info[0].As<Napi::Number>().Uint32Value();
  this->grass_audio_->remove_listener(listener);
}

void GrassAudioWrapper::next(const Napi::CallbackInfo &info) {
  this->grass_audio_->next();
}

void GrassAudioWrapper::previous(const Napi::CallbackInfo &info) {
  this->grass_audio_->previous();
}

void GrassAudioWrapper::skip_to_file(const Napi::CallbackInfo &info) {
  const auto index = info[0].As<Napi::Number>().Int32Value();
  this->grass_audio_->skip_to_file(index);
}

Napi::Value GrassAudioWrapper::get_current_file_index(const Napi::CallbackInfo &info) {
  const auto index = this->grass_audio_->get_current_file_index();

  return Napi::Number::New(info.Env(), static_cast<double>(index));
}



