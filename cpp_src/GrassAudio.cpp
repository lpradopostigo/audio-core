#include <iostream>
#include "GrassAudio.h"

GrassAudio::GrassAudio() {
  BASS_Init(-1, 44100, 0, nullptr, nullptr);
  BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, 1000);
}

void GrassAudio::SetFile(const char *path) {
  const DWORD state = BASS_ChannelIsActive(this->stream);
  if (state == BASS_ACTIVE_PLAYING) {
    std::cerr << "cannot set a file while there is an audio playing" << std::endl;
    return;
  }

  this->stream = BASS_StreamCreateFile(false, path, 0, 0, 0);
}

void GrassAudio::Play() const {
  BASS_ChannelPlay(this->stream, FALSE);
}

void GrassAudio::Pause() const {
  BASS_ChannelPause(this->stream);
}

GrassAudio::~GrassAudio() {
  BASS_Free();
}

void GrassAudio::Stop() const {
  this->Pause();
  this->SetPosition(0);
}

void GrassAudio::SetPosition(double position) const {
  BASS_ChannelSetPosition(this->stream,
                          BASS_ChannelSeconds2Bytes(this->stream, position),
                          BASS_POS_BYTE);

}

double GrassAudio::GetPosition() const {
  const QWORD positionInBytes = BASS_ChannelGetPosition(this->stream, BASS_POS_BYTE);
  return BASS_ChannelBytes2Seconds(this->stream, positionInBytes);
}

void GrassAudio::SetVolume(float value) const {
  BASS_ChannelSetAttribute(this->stream, BASS_ATTRIB_VOL, value);
}

void GrassAudio::SetFileFromMemory(const unsigned char *file, QWORD length) {
  const DWORD state = BASS_ChannelIsActive(this->stream);
  if (state == BASS_ACTIVE_PLAYING) {
    std::cerr << "cannot set a file while there is an audio playing" << std::endl;
    return;
  }

  this->stream = BASS_StreamCreateFile(true, file, 0, length, 0);
  const auto error = BASS_ErrorGetCode();
  if (error != BASS_OK) {
    std::cout << error << std::endl;
  }

}

DWORD GrassAudio::OnPositionReached(SYNCPROC *callback, double position, bool removeListener) const {
  const QWORD positionInBytes = BASS_ChannelSeconds2Bytes(this->stream, position);
  return BASS_ChannelSetSync(this->stream, BASS_SYNC_POS | (removeListener ? BASS_SYNC_ONETIME : 0),
                             positionInBytes,
                             callback, nullptr);
}

DWORD GrassAudio::OnEnd(SYNCPROC *callback, bool removeListener) const {
  return BASS_ChannelSetSync(this->stream, BASS_SYNC_END | (removeListener ? BASS_SYNC_ONETIME : 0),
                             0,
                             callback, nullptr);
}

void GrassAudio::RemoveListener(DWORD listener) const {
  BASS_ChannelRemoveSync(this->stream, listener);
}

DWORD GrassAudio::OnPositionSet(SYNCPROC *callback, bool removeListener) const {
  return BASS_ChannelSetSync(this->stream, BASS_SYNC_SETPOS | (removeListener ? BASS_SYNC_ONETIME : 0),
                             0,
                             callback, nullptr);
}

