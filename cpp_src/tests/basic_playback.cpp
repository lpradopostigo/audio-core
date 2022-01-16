#include "../GrassAudio.h"
#include "./utils.h"
#include <vector>

using namespace std;

int main() {
  {
    log("from memory");
    const auto file1 = read_file("../gapless2.wav");
    const auto file2 = read_file("../gapless3.wav");
    const vector<vector<uint8_t>> files{file1, file2};
    const GrassAudio grass{files};

    grass.play();
    log("playing");
    Sleep(2000);
    grass.pause();
    log("paused");
    Sleep(2000);
    grass.set_position(322);
    grass.play();
    log("seek to 322 and resume");
    Sleep(6000);
  }

  {
    log("from file paths");
    const vector<string> files{"../gapless2.wav", "../gapless3.wav"};
    const GrassAudio grass{files};

    grass.play();
    log("playing");
    Sleep(4000);
    grass.pause();
    log("paused");
    grass.set_position(320);
    log("seek to 320");
    Sleep(1000);
    log("resume");
    grass.play();
  }

  system("Pause");
}