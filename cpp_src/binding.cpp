#include "napi.h"
#include "AudioPlayerWrapper.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  AudioPlayerWrapper::Init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)