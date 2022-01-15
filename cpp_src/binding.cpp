#include "napi.h"
#include "GrassAudioWrapper.h"

Napi::Object init(Napi::Env env, Napi::Object exports) {
  GrassAudioWrapper::init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, init)