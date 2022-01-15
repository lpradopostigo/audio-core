#include <iostream>
#include <utility>
#include "grass_audio.h"
#include "callable_to_pointer.hpp"

void log_error(const std::string &message) {
  const auto error_code = BASS_ErrorGetCode();

  if (error_code != BASS_OK) {
    std::cout << "message:" + message << std::endl << "with error code:" + std::to_string(error_code) << std::endl;
  }
}

grass_audio::grass_audio(std::vector<std::vector<unsigned char>> files, DWORD frequency) {
  this->files = std::move(files);

  BASS_Init(-1, frequency, 0, nullptr, nullptr);
  log_error("init failed");

  BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 2000);
  log_error("config setup failed");

  this->mixer_stream = BASS_Mixer_StreamCreate(frequency, 2, BASS_MIXER_END);
  log_error("mixer stream creation failed");

  const auto c_callback = callable_to_pointer([this](HSYNC, DWORD, DWORD, void *) {
    this->current_file_index++;
    this->load_next_file();
  });

  BASS_ChannelSetSync(this->mixer_stream,
                      BASS_SYNC_END | BASS_SYNC_MIXTIME,
                      0,
                      c_callback,
                      nullptr);
  log_error("ChannelSetSync failed");

  this->load_next_file();
}

size_t grass_audio::resolve_index(int index) {
  if (index < 0) {
    return this->files.size() - 1;
  } else if (index >= this->files.size()) {
    return 0;
  }
}

void grass_audio::play() const {
  BASS_ChannelPlay(this->mixer_stream, FALSE);
}

void grass_audio::skip_to_file(int index) {
  BASS_Mixer_ChannelRemove(this->current_stream);
  this->current_file_index = this->resolve_index(index);
  this->load_next_file();
}

void grass_audio::pause() const {
  BASS_ChannelPause(this->mixer_stream);
}

void grass_audio::flush_mixer() const {
  QWORD channel_position = BASS_Mixer_ChannelGetPosition(this->current_stream, BASS_POS_BYTE);
  BASS_Mixer_ChannelSetPosition(this->current_stream, channel_position, BASS_POS_BYTE);
  BASS_ChannelSetPosition(this->mixer_stream, 0, BASS_POS_BYTE);
}

void grass_audio::stop() {
  this->pause();
  this->flush_mixer();
  BASS_Mixer_ChannelRemove(this->current_stream);

  this->current_file_index = 0;
  this->load_next_file();

}

void grass_audio::set_position(double position) const {
  BASS_Mixer_ChannelSetPosition(this->current_stream,
                                BASS_ChannelSeconds2Bytes(this->current_stream, position),
                                BASS_POS_BYTE | BASS_MIXER_CHAN_NORAMPIN);

  log_error("failed to set position");

}

double grass_audio::get_position() const {
  const QWORD position_in_bytes = BASS_Mixer_ChannelGetPosition(this->current_stream, BASS_POS_BYTE);
  return BASS_ChannelBytes2Seconds(this->current_stream, position_in_bytes);
}

void grass_audio::set_volume(float value) const {
  BASS_ChannelSetAttribute(this->mixer_stream, BASS_ATTRIB_VOL, value);
}

void grass_audio::load_next_file() {
  const auto remaining_files = this->files.size() - this->current_file_index;
  std::cout << "loading file" << std::endl;

  if (remaining_files == 0) {
    std::cout << "playback ended" << std::endl;
    return;
  }

  this->current_stream = BASS_StreamCreateFile(true,
                                               this->files[current_file_index].data(),
                                               0,
                                               this->files[current_file_index].size(),
                                               BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);

  log_error("failed to create stream");

  BASS_Mixer_StreamAddChannel(this->mixer_stream,
                              this->current_stream,
                              BASS_MIXER_CHAN_NORAMPIN);
  log_error("failed to add stream to mixer");

  BASS_ChannelSetPosition(this->mixer_stream, 0, BASS_POS_BYTE);
  log_error("failed to set mixer position to 0");

}
size_t grass_audio::get_current_file_index() const {
  return this->current_file_index;
}

void grass_audio::next() {
  this->skip_to_file(this->current_file_index + 1);
}

void grass_audio::previous() {
  this->skip_to_file(this->current_file_index - 1);
}

DWORD grass_audio::add_listener(grass_audio::Event event,
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
  };

  default:break;
  }

  log_error("failed to set listener");
  return listener;
}

void grass_audio::remove_listener(DWORD listener) const {
  BASS_ChannelRemoveSync(this->mixer_stream, listener);
  log_error("failed to remove listener");
}
