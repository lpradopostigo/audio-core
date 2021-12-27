#include <iostream>
#include "bass.h"
#include "grass_audio.h"
#include <cstdio>
#include <fstream>

std::vector<unsigned char> ReadFile(const char *filename) {
  std::streampos fileSize;
  std::ifstream file(filename, std::ios::binary);

  file.seekg(0, std::ios::end);
  fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<unsigned char> fileData(fileSize);
  file.read((char *) &fileData[0], fileSize);
  return fileData;
}

int main() {
  BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 10000);
//  const auto file = ReadFile("../1.wav");
  auto player = new grass_audio();
  player->set_file("../1.wav");
//  player->set_file_from_memory(file.data(), file.size());

  auto callback = [](){std::cout << "gaaa" << std::endl;};
  player->on_position_set(callback);
  player->set_position(50);

  player->play();

//  Sleep(10000);

//  player->set_position(0);
//  player->remove_listener(listener);

  system("pause");
  return 0;
}