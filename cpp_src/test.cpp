#include <iostream>
#include "bass.h"
#include "GrassAudio.h"
#include <cstdio>

#include <fstream>
#include <vector>

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

using namespace std;

void Callback(HSYNC,
              DWORD,
              DWORD,
              void *) {
  cout << "gaaa" << endl;
}

int main() {
  BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 10000);
//  const auto file = ReadFile("../1.wav");
  auto player = new GrassAudio();
  player->SetFile("../1.wav");
//  player->SetFileFromMemory(file.data(), file.size());
//  player->SetPosition(50);

  auto listener = player->OnPositionSet(&Callback);
  player->Play();

  Sleep(10000);

  player->SetPosition(0);
//  player->RemoveListener(listener);

  system("pause");
  return 0;
}