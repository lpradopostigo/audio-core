#pragma once
#include "bass.h"
#include "bassmix.h"
#include <functional>
#include <type_traits>
#include <iostream>
#include <string>
#include <utility>
#include "callable_to_pointer.hpp"

template<typename T>
concept BinaryOrFilePath = std::is_same_v<T, std::string>
    || std::is_same_v<T, std::vector<uint8_t>>;

enum GrassAudioEvent {
  POSITION_REACHED, // FIX
  END,
};

template<BinaryOrFilePath T = std::string>
class GrassAudio {
public:
  explicit GrassAudio(std::vector<T> files, DWORD frequency = 44100);
  explicit GrassAudio(DWORD frequency = 44100);

  ~GrassAudio();

  [[maybe_unused]] void play() const;
  [[maybe_unused]] void pause() const;
  [[maybe_unused]] void stop();
  [[maybe_unused]] void set_position(double position) const;
  [[maybe_unused]] [[nodiscard]]  double get_position() const;
  [[maybe_unused]] void set_volume(float value) const;
  [[maybe_unused]] void skip_to_file(int index);
  [[maybe_unused]] void next();
  [[maybe_unused]] void previous();
  [[maybe_unused]] [[nodiscard]] DWORD add_listener(GrassAudioEvent event,
                                                    const std::function<void()> &callback,
                                                    bool remove_on_trigger = false, double position = 0) const;

  [[maybe_unused]] [[nodiscard]] size_t get_current_file_index() const;

  [[maybe_unused]] void remove_listener(DWORD listener) const;
  [[maybe_unused]] void set_files(std::vector<T> files);
  [[maybe_unused]] [[nodiscard]] QWORD get_length() const;
  [[maybe_unused]] [[nodiscard]] DWORD get_status() const;

private:
  std::vector<T> files_{};

  DWORD current_stream = 0;
  size_t current_file_index_ = 0;
  HSTREAM mixer_stream = 0;
  HSYNC end_sync = 0;

  void load_next_file();
  void flush_mixer() const;
  size_t resolve_index(int index);
  static void log_bass_error(const std::string &message);
  static void log(const std::string &message);

  void common_init_routine(DWORD frequency);
};

template<BinaryOrFilePath T>
void GrassAudio<T>::log_bass_error(const std::string &message) {
  const auto error_code = BASS_ErrorGetCode();

  if (error_code != BASS_OK) {
    std::cout << "|GrassAudio| " << "message:" + message << std::endl
              << "with error code:" + std::to_string(error_code) << std::endl;
  }
}

template<BinaryOrFilePath T>
void GrassAudio<T>::log(const std::string &message) {
  std::cout << "|GrassAudio| " << "message:" + message << std::endl;
}

template<BinaryOrFilePath T>
GrassAudio<T>::GrassAudio(std::vector<T> files, DWORD frequency) {
  this->files_ = std::move(files);
  this->common_init_routine(frequency);
  this->load_next_file();
}

template<BinaryOrFilePath T>
GrassAudio<T>::GrassAudio(DWORD frequency) {
  this->common_init_routine(frequency);
}

template<BinaryOrFilePath T>
void GrassAudio<T>::common_init_routine(DWORD frequency) {
  BASS_Init(1, frequency, BASS_DEVICE_REINIT, nullptr, nullptr);
  log_bass_error("BASS_Init failed");

  BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 2000);
  log_bass_error("config setup failed");

  this->mixer_stream = BASS_Mixer_StreamCreate(frequency, 2, BASS_MIXER_END);
  log_bass_error("mixer stream creation failed");

  const auto c_callback = callable_to_pointer([this](HSYNC, DWORD, DWORD, void *) -> void {
    this->current_file_index_++;
    this->load_next_file();
  });

  this->end_sync = BASS_ChannelSetSync(this->mixer_stream,
                                       BASS_SYNC_END | BASS_SYNC_MIXTIME,
                                       0,
                                       reinterpret_cast<SYNCPROC (__stdcall *)>(c_callback),
                                       nullptr);
  log_bass_error("ChannelSetSync failed");
}

template<BinaryOrFilePath T>
size_t GrassAudio<T>::resolve_index(int index) {
  if (index < 0) {
    return this->files_.size() - 1;
  } else if (index >= this->files_.size()) {
    return 0;
  } else {
    return index;
  }
}

template<BinaryOrFilePath T>
void GrassAudio<T>::play() const {
  if (this->current_stream != 0) {
    BASS_ChannelPlay(this->mixer_stream, FALSE);
    log_bass_error("failed to play");
  }
}

template<BinaryOrFilePath T>
void GrassAudio<T>::skip_to_file(int index) {
  BASS_Mixer_ChannelRemove(this->current_stream);
  this->current_file_index_ = this->resolve_index(index);
  this->load_next_file();
}

template<BinaryOrFilePath T>
void GrassAudio<T>::pause() const {
  BASS_ChannelPause(this->mixer_stream);
}

template<BinaryOrFilePath T>
void GrassAudio<T>::flush_mixer() const {
  if (current_stream != 0) {
    QWORD channel_position = BASS_Mixer_ChannelGetPosition(this->current_stream, BASS_POS_BYTE);
    BASS_Mixer_ChannelSetPosition(this->current_stream, channel_position, BASS_POS_BYTE);
  }
  BASS_ChannelSetPosition(this->mixer_stream, 0, BASS_POS_BYTE);
}

template<BinaryOrFilePath T>
void GrassAudio<T>::stop() {
  this->pause();
  this->flush_mixer();
  BASS_Mixer_ChannelRemove(this->current_stream);

  this->current_file_index_ = 0;
  this->load_next_file();

}

template<BinaryOrFilePath T>
void GrassAudio<T>::set_position(double position) const {
  BASS_Mixer_ChannelSetPosition(this->current_stream,
                                BASS_ChannelSeconds2Bytes(this->current_stream, position),
                                BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN | BASS_POS_MIXER_RESET);

  log_bass_error("failed to set position");

}

template<BinaryOrFilePath T>
double GrassAudio<T>::get_position() const {
  const QWORD position_in_bytes = BASS_Mixer_ChannelGetPosition(this->current_stream, BASS_POS_BYTE);
  return BASS_ChannelBytes2Seconds(this->current_stream, position_in_bytes);
}

template<BinaryOrFilePath T>
void GrassAudio<T>::set_volume(float value) const {
  BASS_ChannelSetAttribute(this->mixer_stream, BASS_ATTRIB_VOL, value);
}

template<BinaryOrFilePath T>
void GrassAudio<T>::load_next_file() {
  const auto remaining_files = this->files_.size() - this->current_file_index_;
  if (remaining_files == 0) {
    log("playback ended");
    return;
  }

  log("loading file");

  if constexpr(std::is_same_v<T, std::string>) {
    this->current_stream = BASS_StreamCreateFile(false,
                                                 this->files_[current_file_index_].c_str(),
                                                 0,
                                                 0,
                                                 BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);

  } else if (std::is_same_v<T, std::vector<uint8_t>>) {
    this->current_stream = BASS_StreamCreateFile(true,
                                                 this->files_[current_file_index_].data(),
                                                 0,
                                                 this->files_[current_file_index_].size(),
                                                 BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);
  }

  log_bass_error("failed to create stream");

  BASS_Mixer_StreamAddChannel(this->mixer_stream,
                              this->current_stream,
                              BASS_MIXER_CHAN_NORAMPIN | BASS_STREAM_AUTOFREE);
  log_bass_error("failed to add stream to mixer");

  BASS_ChannelSetPosition(this->mixer_stream, 0, BASS_POS_BYTE);
  log_bass_error("failed reset mixer position");

}

template<BinaryOrFilePath T>
size_t GrassAudio<T>::get_current_file_index() const {
  return this->current_file_index_;
}

template<BinaryOrFilePath T>
void GrassAudio<T>::next() {
  this->skip_to_file(this->current_file_index_ + 1);
}

template<BinaryOrFilePath T>
void GrassAudio<T>::previous() {
  this->skip_to_file(this->current_file_index_ - 1);
}

template<BinaryOrFilePath T>
DWORD GrassAudio<T>::add_listener(GrassAudioEvent event,
                                  const std::function<void()> &callback,
                                  bool remove_on_trigger, double position) const {
  const auto c_callback = callable_to_pointer([callback](HSYNC, DWORD, DWORD, void *) { callback(); });
  const DWORD one_time = remove_on_trigger ? BASS_SYNC_ONETIME : 0;

  DWORD listener = 0;

  switch (event) {
  case POSITION_REACHED: {
    const QWORD position_in_bytes = BASS_ChannelSeconds2Bytes(this->mixer_stream, position);
    listener = BASS_ChannelSetSync(this->mixer_stream, BASS_SYNC_POS | BASS_SYNC_MIXTIME | one_time,
                                   position_in_bytes,
                                   c_callback, nullptr);
    break;
  }

  case END: {
    listener = BASS_ChannelSetSync(this->mixer_stream, BASS_SYNC_END | BASS_SYNC_MIXTIME | one_time,
                                   0,
                                   c_callback, nullptr);
    break;
  }

  default:break;
  }

  log_bass_error("failed to set end_sync");
  return listener;
}

template<BinaryOrFilePath T>
void GrassAudio<T>::remove_listener(DWORD listener) const {
  BASS_ChannelRemoveSync(this->mixer_stream, listener);
  log_bass_error("failed to remove end_sync");
}

template<BinaryOrFilePath T>
GrassAudio<T>::~GrassAudio() {
  this->pause();
  BASS_StreamFree(this->mixer_stream);
  log_bass_error("failed to free mixer stream");
}

template<BinaryOrFilePath T>
void GrassAudio<T>::set_files(std::vector<T> files) {
  this->pause();
  this->flush_mixer();
  if (current_stream != 0)
    BASS_Mixer_ChannelRemove(this->current_stream);
  this->files_ = std::move(files);
  this->current_file_index_ = 0;
  this->current_stream = 0;

  this->load_next_file();
}

template<BinaryOrFilePath T>
QWORD GrassAudio<T>::get_length() const {
  const auto length_in_bytes = BASS_ChannelGetLength(this->current_stream, BASS_POS_BYTE);
  if (BASS_ErrorGetCode() != BASS_OK) {
    return 0;
  }

  const auto length_in_seconds = BASS_ChannelBytes2Seconds(this->current_stream, length_in_bytes);
  log_bass_error("failed to convert length byte to seconds");
  return length_in_seconds;
}

template<BinaryOrFilePath T>
DWORD GrassAudio<T>::get_status() const {
  return BASS_ChannelIsActive(this->mixer_stream);
}
