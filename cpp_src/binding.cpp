#include "napi.h"
#include "grass_audio_wrapper.h"

Napi::Object init(Napi::Env env, Napi::Object exports) {
  grass_audio_wrapper::init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, init)