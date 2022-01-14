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
    this->current_position++;
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

void grass_audio::play() const {
  BASS_ChannelPlay(this->mixer_stream, FALSE);
}

void grass_audio::pause() const {
  BASS_Mixer_ChannelRemove(this->current_stream);
}

void grass_audio::stop() {
  this->pause();
  this->current_position = 0;
  this->load_next_file();
}

void grass_audio::set_position(double position) const {
  BASS_Mixer_ChannelSetPosition(this->current_stream,
                                BASS_ChannelSeconds2Bytes(this->current_stream, position),
                                BASS_POS_BYTE);

  log_error("failed to set position");

}

double grass_audio::get_position() const {
  const QWORD position_in_bytes = BASS_Mixer_ChannelGetPosition(this->mixer_stream, BASS_POS_BYTE);
  return BASS_ChannelBytes2Seconds(this->mixer_stream, position_in_bytes);
}

//void grass_audio::set_volume(float value) const {
//  BASS_ChannelSetAttribute(this->stream, BASS_ATTRIB_VOL, value);
//}


//DWORD grass_audio::on_position_reached(const std::function<void()> &callback,
//                                       double position,
//                                       bool remove_listener) const {
//  const auto c_callback = callable_to_pointer([callback](HSYNC, DWORD, DWORD, void *) { callback(); });
//  const QWORD position_in_bytes = BASS_ChannelSeconds2Bytes(this->stream, position);
//  return BASS_ChannelSetSync(this->stream, BASS_SYNC_POS | (remove_listener ? BASS_SYNC_ONETIME : 0),
//                             position_in_bytes,
//                             c_callback, nullptr);
//}
//
//DWORD grass_audio::on_end(const std::function<void()> &callback, bool remove_listener) const {
//  const auto c_callback = callable_to_pointer([callback](HSYNC, DWORD, DWORD, void *) { callback(); });
//  return BASS_ChannelSetSync(this->stream, BASS_SYNC_END | (remove_listener ? BASS_SYNC_ONETIME : 0),
//                             0,
//                             c_callback, nullptr);
//}
//
//void grass_audio::remove_listener(DWORD listener) const {
//  BASS_ChannelRemoveSync(this->file_streams[0], listener);
//}

//DWORD grass_audio::on_position_set(const std::function<void()> &callback, bool remove_listener) const {
//  const auto c_callback = callable_to_pointer([callback](HSYNC, DWORD, DWORD, void *) { callback(); });
//  const auto listener = BASS_ChannelSetSync(this->stream, BASS_SYNC_SETPOS | (remove_listener ? BASS_SYNC_ONETIME : 0),
//                                            0,
//                                            c_callback, nullptr);
//  return listener;
//}

void grass_audio::load_next_file() {
  const auto remaining_files = this->files.size() - this->current_position;

  if (remaining_files == 0)
    return;

  this->current_stream = BASS_StreamCreateFile(true,
                                               this->files[current_position].data(),
                                               0,
                                               this->files[current_position].size(),
                                               BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);

  BASS_Mixer_StreamAddChannel(this->mixer_stream,
                              this->current_stream,
                              BASS_STREAM_AUTOFREE | BASS_MIXER_CHAN_NORAMPIN);
  BASS_ChannelSetPosition(this->mixer_stream, 0, BASS_POS_BYTE);

}
