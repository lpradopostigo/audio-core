#include <iostream>
#include "bass.h"
#include "grass_audio.h"
#include <cstdio>
#include <fstream>
#include <cassert>
#include <iterator>
#include "bassmix.h"
using namespace std;

void log_error() {
  cout << BASS_ErrorGetCode() << endl;
}

std::vector<BYTE> read_file(const char *filename) {
  // open the file:
  std::ifstream file(filename, std::ios::binary);

  // Stop eating new lines in binary mode!!!
  file.unsetf(std::ios::skipws);

  // get its size:
  std::streampos fileSize;

  file.seekg(0, std::ios::end);
  fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  // reserve capacity
  std::vector<BYTE> vec;
  vec.reserve(fileSize);

  // read the data:
  vec.insert(vec.begin(),
             std::istream_iterator<BYTE>(file),
             std::istream_iterator<BYTE>());

  return vec;
}

int main() {
  auto file1 = read_file("../gapless2.wav");
  auto file2 = read_file("../gapless3.wav");
  vector<vector<BYTE>> files{};
  files.push_back(file1);
  files.push_back(file2);

  const auto grass = new grass_audio(files);
  grass->set_position(320);
  grass->play();

//  Sleep(4000);
  cout << "aea" << endl;
//  grass->stop();
//  Sleep(1000);
//  grass->play();

  system("Pause");

}