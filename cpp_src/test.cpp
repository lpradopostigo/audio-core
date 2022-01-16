#include <iostream>
#include "bass.h"
#include "GrassAudio.h"
#include <cstdio>
#include <fstream>
#include <cassert>
#include <iterator>
#include "bassmix.h"
using namespace std;

void log_error() {
  cout << BASS_ErrorGetCode() << endl;
}

std::vector<uint8_t> read_file(const char *filename) {
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
  std::vector<uint8_t> vec;
  vec.reserve(fileSize);

  // read the data:
  vec.insert(vec.begin(),
             std::istream_iterator<BYTE>(file),
             std::istream_iterator<BYTE>());

  return vec;
}

int main() {
//  auto file1 = read_file("../gapless2.wav");
//  auto file2 = read_file("../gapless3.wav");
//  vector<vector<uint8_t>> files{};
//  files.push_back(file1);
//  files.push_back(file2);
  vector<string> files{"../gapless2.wav","../gapless3.wav"};

  const auto grass = new GrassAudio(files);
  grass->set_position(320);
  grass->play();

//  const auto
//      listener = grass->add_listener(GrassAudio<vector<uint8_t>>::END, [] { cout << "reached" << endl; }, false, 310);
  Sleep(2000);


  Sleep(8000);

//  grass->set_position(250);

//  cout << grass->get_current_file_index() << endl;


  system("Pause");

}