#pragma once
#include <string>
#include "bass.h"

class AudioPlayer {
 public:
  AudioPlayer();
  ~AudioPlayer();
  void SetFile(const std::string &path);
  void Play() const;
  void Pause() const;
  void Stop() const;
  void SetPosition(double pos) const;
  void SetVolume(float value) const;
  [[nodiscard]] double GetPosition() const;

 private:
  HSTREAM stream = 0;
};

