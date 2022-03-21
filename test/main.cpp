
extern "C"{
#include "log.h"
}
#include "bass.h"


int main(){
  log_info("aea");
  BASS_Init(1, 44000, BASS_DEVICE_REINIT, nullptr, nullptr);
}