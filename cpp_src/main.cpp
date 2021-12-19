#include <iostream>
#include "bass.h"
#include "AudioPlayer.h"

using namespace std;

int main() {
  BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 10000);
  auto player = new AudioPlayer();
  player->SetFile("1.wav");
  player->SetPosition(50);
  player->Play();
  Sleep(1000);
  auto player2 = new AudioPlayer();
  player2->SetFile("1.wav");
  player2->Play();

  system("pause");
  return 0;
}