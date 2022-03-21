#include <iostream>
#include "bass.h"
#include "../include/grass_audio.hpp"
#include <fstream>
#include <iterator>
using namespace std;

std::vector<uint8_t> read_file(const char* filename) {
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
	vector<string> files1{"../../short.wav", "../../gapless3.wav"};
	vector<string> files2{"../../gapless1.wav", "../../gapless2.wav"};

	GrassAudio grass{};
	grass.set_files(files2);

	grass.play();
	Sleep(1000);

	Sleep(1000);

	Sleep(1000);

	Sleep(1000);
	system("Pause");

}