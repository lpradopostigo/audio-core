#include <fstream>
#include <iterator>
#include <iostream>
#include <string>

std::vector<uint8_t> read_file(const char *filename) {
  std::ifstream file(filename, std::ios::binary);
  file.unsetf(std::ios::skipws);
  std::streampos fileSize;
  file.seekg(0, std::ios::end);
  fileSize = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<uint8_t> vec;
  vec.reserve(fileSize);
  vec.insert(vec.begin(),
             std::istream_iterator<uint8_t>(file),
             std::istream_iterator<uint8_t>());
  return vec;
}

void log(const std::string& message){
  std::cout << message << std::endl;
}
