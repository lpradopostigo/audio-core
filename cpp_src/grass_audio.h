#pragma once
#include "bass.h"
#include "bassmix.h"
#include <functional>

class grass_audio {
public:
  explicit grass_audio(std::vector<std::vector<unsigned char>> files, DWORD frequency = 44100);
  ~grass_audio() = default;

  [[maybe_unused]] void play() const;
  [[maybe_unused]] void pause() const;
  [[maybe_unused]] void stop();
  [[maybe_unused]] void set_position(double position) const;
  [[maybe_unused]] [[nodiscard]]  double get_position() const;
  [[maybe_unused]] void set_volume(float value) const;
  [[maybe_unused]] void skip_to_file(int index);
  [[maybe_unused]] void next();

  [[maybe_unused]] [[nodiscard]] size_t get_current_file_index() const;

  [[maybe_unused]] void remove_listener(DWORD listener) const; //TODO
  [[maybe_unused]] DWORD on_position_reached(const std::function<void()> &callback,//TODO
                                             double position,
                                             bool remove_listener = false) const;
  [[maybe_unused]] DWORD on_end(const std::function<void()> &callback, bool remove_listener = false) const;//TODO
  [[maybe_unused]] DWORD on_position_set(const std::function<void()> &callback,
                                         bool remove_listener = false) const;//TODO

private:
  std::vector<std::vector<unsigned char>> files{};

  DWORD current_stream = 0;
  size_t current_file_index = 0;
  HSTREAM mixer_stream = 0;

  void load_next_file();
  void flush_mixer() const;
  size_t resolve_index(int index);
  void previous();
};

