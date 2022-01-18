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
  vector<string> files1{"../short.wav/**/", "../gapless3.wav"};
  vector<string> files2{"../gapless1.wav", "../gapless2.wav"};

  const auto grass = new GrassAudio();
  grass->set_files(files1);

  grass->play();
  cout << grass->get_length() << endl;
  cout << grass->get_position() << endl;
  Sleep(1000);
  cout << grass->get_position() << endl;

  Sleep(1000);
  cout << grass->get_position() << endl;

  Sleep(1000);
  cout << grass->get_position() << endl;


  Sleep(1000);
  grass->set_position(2);
  cout << grass->get_position() << endl;




  system("Pause");
  delete grass;

}