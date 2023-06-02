use grass_player_sys::*;
use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use std::path::Path;
use std::ptr::null;

#[derive(Debug, Clone)]
pub enum SampleRate {
    Hz44100 = 44100,
    Hz48000 = 48000,
}

impl From<GpSampleRate> for SampleRate {
    fn from(sample_rate: GpSampleRate) -> Self {
        match sample_rate {
            GP_SAMPLE_RATE_44100 => Self::Hz44100,
            GP_SAMPLE_RATE_48000 => Self::Hz48000,
            _ => unreachable!(),
        }
    }
}

impl From<SampleRate> for GpSampleRate {
    fn from(sample_rate: SampleRate) -> Self {
        match sample_rate {
            SampleRate::Hz44100 => GP_SAMPLE_RATE_44100,
            SampleRate::Hz48000 => GP_SAMPLE_RATE_48000,
        }
    }
}

#[derive(Debug, Clone)]
pub enum PlaybackState {
    Stopped,
    Playing,
    Paused,
}

impl From<GpPlaybackState> for PlaybackState {
    fn from(playback_state: GpPlaybackState) -> Self {
        match playback_state {
            GP_PLAYBACK_STATE_STOPPED => Self::Stopped,
            GP_PLAYBACK_STATE_PLAYING => Self::Playing,
            GP_PLAYBACK_STATE_PAUSED => Self::Paused,
            _ => unreachable!(),
        }
    }
}

#[derive(Debug, thiserror::Error)]
pub enum PlayerError {
    #[error("failed to initialize player")]
    InitFailed,
    #[error("failed to close player")]
    CloseFailed,
    #[error("invalid path")]
    InvalidPath,
    #[error("internal error")]
    Internal,
}

pub type PlayerResult<T> = Result<T, PlayerError>;

pub struct Player;

impl Player {
    pub fn init(sample_rate: SampleRate) -> PlayerResult<()> {
        unsafe {
            if gp_init(sample_rate.into()) == GP_RESULT_OK {
                Ok(())
            } else {
                Err(PlayerError::InitFailed)
            }
        }
    }

    pub fn close() -> PlayerResult<()> {
        unsafe {
            if gp_close() == GP_RESULT_OK {
                Ok(())
            } else {
                Err(PlayerError::CloseFailed)
            }
        }
    }

    pub fn set_sources<T: AsRef<Path>>(paths: &[T]) -> PlayerResult<()> {
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
            if gp_set_sources(paths_ptr.as_ptr() as *mut *const c_char, paths.len()) == GP_RESULT_OK
            {
                Ok(())
            } else {
                Err(PlayerError::Internal)
            }
        }
    }

    pub fn play() {
        unsafe {
            gp_play();
        }
    }

    pub fn pause() {
        unsafe {
            gp_pause();
        }
    }

    pub fn stop() {
        unsafe {
            gp_stop();
        }
    }

    pub fn seek(position: f64) {
        unsafe {
            gp_seek(position);
        }
    }

    pub fn skip_to(index: usize) {
        unsafe {
            gp_skip_to(index);
        }
    }
    pub fn playback_state() -> PlaybackState {
        unsafe { gp_get_playback_state().into() }
    }

    pub fn source_path() -> PlayerResult<Option<String>> {
        unsafe {
            let path_ptr = gp_get_source_path();

            if path_ptr.is_null() {
                Ok(None)
            } else {
                Ok(Some(
                    CStr::from_ptr(path_ptr)
                        .to_str()
                        .map_err(|_| PlayerError::InvalidPath)?
                        .to_string(),
                ))
            }
        }
    }

    pub fn source_index() -> usize {
        unsafe { gp_get_source_index() }
    }

    pub fn sources_size() -> usize {
        unsafe { gp_get_sources_size() }
    }

    pub fn source_position() -> f64 {
        unsafe { gp_get_source_position() }
    }

    pub fn source_duration() -> f64 {
        unsafe { gp_get_source_duration() }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn basic_playback() -> PlayerResult<()> {
        let manifest_dir_path = Path::new(env!("CARGO_MANIFEST_DIR"));
        let sample_files_path = manifest_dir_path.join("../../test/sample-files");

        Player::init(SampleRate::Hz44100).expect("failed to initialize player");

        let track1 = sample_files_path.join("01_Ghosts_I.flac");
        let track2 = sample_files_path.join("24_Ghosts_III.flac");
        let track3 = sample_files_path.join("25_Ghosts_III.flac");

        let tracks = vec![track1, track2, track3];

        Player::set_sources(&tracks)?;

        Player::play();

        std::thread::sleep(std::time::Duration::from_secs(5));

        Player::pause();

        std::thread::sleep(std::time::Duration::from_secs(5));

        Player::close().expect("failed to close player");
        Ok(())
    }
}
