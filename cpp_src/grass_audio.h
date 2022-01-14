#pragma once
#include "bass.h"
#include "bassmix.h"
#include <functional>

class grass_audio {
public:
  explicit grass_audio(std::vector<std::vector<unsigned char>> files, DWORD frequency = 44100);
  ~grass_audio();

  [[maybe_unused]] void set_file(const char *path);
  [[maybe_unused]] void set_file_from_memory(const unsigned char *file, QWORD length);
  [[maybe_unused]] void play() const;
  [[maybe_unused]] void pause() const;
  [[maybe_unused]] void stop() const;
  [[maybe_unused]] void set_position(double position) const;
  [[maybe_unused]] void set_volume(float value) const;
  [[maybe_unused]] [[nodiscard]]  double get_position() const;
  [[maybe_unused]] [[nodiscard]]  double get_length() const;

  [[maybe_unused]] void remove_listener(DWORD listener) const;
  [[maybe_unused]] DWORD on_position_reached(const std::function<void()> &callback,
                                             double position,
                                             bool remove_listener = false) const;
  [[maybe_unused]] DWORD on_end(const std::function<void()> &callback, bool remove_listener = false) const;
  [[maybe_unused]] DWORD on_position_set(const std::function<void()> &callback, bool remove_listener = false) const;

private:
  HSTREAM stream = 0;
  std::vector<std::vector<unsigned char>> files{};
  std::vector<HSTREAM> file_streams{};

  int current_position = 0;

  HSTREAM mixer_stream = 0;

  void load_next_file();

};

