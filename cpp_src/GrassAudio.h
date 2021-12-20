#pragma once
#include "bass.h"

class GrassAudio {
public:
  GrassAudio();
  ~GrassAudio();
  [[maybe_unused]] void SetFile(const char *path);
  [[maybe_unused]] void SetFileFromMemory(const unsigned char *file, QWORD length);
  [[maybe_unused]] void Play() const;
  [[maybe_unused]] void Pause() const;
  [[maybe_unused]] void Stop() const;
  [[maybe_unused]] void SetPosition(double position) const;
  [[maybe_unused]] void SetVolume(float value) const;
  [[maybe_unused]] [[nodiscard]]  double GetPosition() const;
  [[maybe_unused]] void RemoveListener(DWORD listener) const;
  [[maybe_unused]] DWORD OnPositionReached(SYNCPROC *callback, double position, bool removeListener = false) const;
  [[maybe_unused]] DWORD OnEnd(SYNCPROC *callback, bool removeListener = false) const;
  [[maybe_unused]] DWORD OnPositionSet(SYNCPROC *callback, bool removeListener = false) const;

private:
  HSTREAM stream = 0;
};

