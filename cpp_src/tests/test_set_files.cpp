#include "../GrassAudio.h"
#include "./utils.h"
#include <vector>

using namespace std;

int main() {
  const vector<string> files1{"../gapless2.wav", "../gapless3.wav"};
  const vector<string> files2{"../gapless1.wav", "../gapless2.wav"};

  GrassAudio grass{files2};
  grass.play();
  Sleep(4000);

  grass.set_files(files1);
  grass.play();
  Sleep(4000);
  grass.set_files(files2);
  grass.play();

  system("Pause");
}