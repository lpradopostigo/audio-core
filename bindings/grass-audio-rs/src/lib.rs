use grass_audio_sys::*;
use std::ffi::CString;
use std::os::raw::c_char;
use std::path::Path;
use std::ptr::null;

#[derive(Debug, Clone)]
pub enum SampleRate {
    Hz44100 = 44100,
    Hz48000 = 48000,
    Hz88200 = 88200,
    Hz96000 = 96000,
    Hz192000 = 192000,
}

#[derive(Debug, thiserror::Error)]
pub enum AudioOutputError {
    #[error("failed to initialize audio output")]
    InitFailed,
    #[error("failed to close audio output")]
    CloseFailed,
}

pub type AudioOutputResult<T> = Result<T, AudioOutputError>;

pub struct AudioOutput;

impl AudioOutput {
    pub fn init(sample_rate: SampleRate) -> AudioOutputResult<()> {
        let result;
        unsafe {
            result = ga_audio_output_init(sample_rate as u32);
        }

        if result == GA_RESULT_OK {
            Ok(())
        } else {
            Err(AudioOutputError::InitFailed)
        }
    }

    pub fn close() -> AudioOutputResult<()> {
        let result;
        unsafe {
            result = ga_audio_output_close();
        }

        if result == GA_RESULT_OK {
            Ok(())
        } else {
            Err(AudioOutputError::CloseFailed)
        }
    }
}

#[derive(Debug, Clone)]
pub enum PlaybackState {
    Stopped,
    Playing,
    Paused,
}

#[derive(Debug, thiserror::Error)]
pub enum PlayerError {
    #[error("invalid path")]
    InvalidPath,
    #[error("internal error")]
    Internal,
}

type PlayerResult<T> = Result<T, PlayerError>;

pub struct Player {
    inner: *mut GaPlayer,
}

impl Player {
    pub fn new(sample_rate: SampleRate) -> PlayerResult<Self> {
        let inner;
        unsafe {
            inner = ga_new_player(sample_rate as u32);
        }

        if inner.is_null() {
            Err(PlayerError::Internal)
        } else {
            Ok(Self { inner })
        }
    }

    pub fn set_sources<T: AsRef<Path>>(&self, paths: &[T]) -> PlayerResult<()> {
        let mut cstr_paths = Vec::with_capacity(paths.len());

        for path in paths.iter() {
            if let Some(path_str) = path.as_ref().as_os_str().to_str() {
                cstr_paths.push(CString::new(path_str).map_err(|_| PlayerError::InvalidPath)?);
            } else {
                return Err(PlayerError::InvalidPath);
            }
        }

        let mut paths_ptr: Vec<_> = cstr_paths.iter().map(|track| track.as_ptr()).collect();

        paths_ptr.push(null());

        unsafe {
            if ga_player_set_sources(
                self.inner,
                paths_ptr.as_ptr() as *mut *const c_char,
                paths.len(),
            ) != GA_RESULT_OK
            {
                return Err(PlayerError::Internal);
            }
        }

        Ok(())
    }

    pub fn play(&self) {
        unsafe {
            ga_player_play(self.inner);
        }
    }

    pub fn pause(&self) {
        unsafe {
            ga_player_pause(self.inner);
        }
    }

    pub fn stop(&self) {
        unsafe {
            ga_player_stop(self.inner);
        }
    }

    pub fn seek(&self, position: f64) {
        unsafe {
            ga_player_seek(self.inner, position);
        }
    }

    pub fn skip_to(&self, index: usize) {
        unsafe {
            ga_player_skip_to(self.inner, index);
        }
    }

    pub fn playback_state(&self) -> PlaybackState {
        let state;
        unsafe {
            state = ga_player_get_playback_state(self.inner);
        }

        match state {
            GA_PLAYER_PLAYBACK_STATE_STOPPED => PlaybackState::Stopped,
            GA_PLAYER_PLAYBACK_STATE_PLAYING => PlaybackState::Playing,
            GA_PLAYER_PLAYBACK_STATE_PAUSED => PlaybackState::Paused,
            _ => unreachable!(),
        }
    }

    pub fn source_path(&self) -> PlayerResult<Option<String>> {
        let path;
        unsafe {
            let path_ptr = ga_player_get_source_path(self.inner);

            if path_ptr.is_null() {
                return Ok(None);
            } else {
                path = std::ffi::CStr::from_ptr(path_ptr)
            }
        }

        Ok(Some(
            path.to_str()
                .map_err(|_| PlayerError::InvalidPath)?
                .to_string(),
        ))
    }

    pub fn source_index(&self) -> usize {
        unsafe { ga_player_get_source_index(self.inner) }
    }

    pub fn sources_size(&self) -> usize {
        unsafe { ga_player_get_sources_size(self.inner) }
    }

    pub fn source_position(&self) -> f64 {
        unsafe { ga_player_get_source_position(self.inner) }
    }

    pub fn source_duration(&self) -> f64 {
        unsafe { ga_player_get_source_duration(self.inner) }
    }
}

impl Drop for Player {
    fn drop(&mut self) {
        unsafe {
            ga_free_player(self.inner);
        }
    }
}

unsafe impl Send for Player {}
unsafe impl Sync for Player {}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn basic_playback() -> PlayerResult<()> {
        let manifest_dir_path = Path::new(env!("CARGO_MANIFEST_DIR"));
        let sample_files_path = manifest_dir_path.join("../../test/sample-files");

        AudioOutput::init(SampleRate::Hz44100).expect("failed to initialize audio output");

        let player = Player::new(SampleRate::Hz44100)?;

        let track1 = sample_files_path.join("01_Ghosts_I.flac");
        let track2 = sample_files_path.join("24_Ghosts_III.flac");
        let track3 = sample_files_path.join("25_Ghosts_III.flac");

        let tracks = vec![track1, track2, track3];

        player.set_sources(&tracks)?;

        player.play();

        std::thread::sleep(std::time::Duration::from_secs(5));

        player.pause();

        std::thread::sleep(std::time::Duration::from_secs(5));

        AudioOutput::close().expect("failed to close audio output");
        Ok(())
    }
}
