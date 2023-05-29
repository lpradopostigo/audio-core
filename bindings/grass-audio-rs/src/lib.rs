use grass_audio_sys::*;
use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use std::path::Path;
use std::ptr::null;
use std::sync::Mutex;

#[derive(Debug, Clone)]
pub enum SampleRate {
    Hz44100 = 44100,
    Hz48000 = 48000,
    Hz88200 = 88200,
    Hz96000 = 96000,
    Hz192000 = 192000,
}

#[derive(Debug, Clone)]
pub enum PlaybackState {
    Stopped,
    Playing,
    Paused,
}

#[derive(Debug, thiserror::Error)]
pub enum GrassAudioError {
    #[error("Failed to initialize grass audio")]
    InitFailed,
    #[error("Failed to terminate grass audio")]
    TerminateFailed,
    #[error("Failed to set playlist")]
    InvalidPath,
    #[error("Failed to lock resource")]
    LockResourceFailed,
    #[error("Unknown error")]
    Unknown,
}

type GrassAudioResult<T> = Result<T, GrassAudioError>;

// since the underlying  library is not thread-safe, we need to lock it
static PLAYER_MUTEX: Mutex<()> = Mutex::new(());

pub struct GrassAudio;

impl GrassAudio {
    pub fn init(sample_rate: SampleRate) -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            let result;
            unsafe {
                result = ga_init(sample_rate as u32);
            }

            if result == GA_RESULT_OK {
                Ok(())
            } else {
                Err(GrassAudioError::InitFailed)
            }
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn terminate() -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            let result;
            unsafe {
                result = ga_terminate();
            }

            if result == GA_RESULT_OK {
                Ok(())
            } else {
                Err(GrassAudioError::TerminateFailed)
            }
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn set_playlist<T: AsRef<Path>>(playlist: &[T]) -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            let mut cstr_playlist = Vec::with_capacity(playlist.len());

            for path in playlist.iter() {
                if let Some(path_str) = path.as_ref().as_os_str().to_str() {
                    cstr_playlist
                        .push(CString::new(path_str).map_err(|_| GrassAudioError::InvalidPath)?);
                } else {
                    return Err(GrassAudioError::InvalidPath);
                }
            }

            let mut ptr_playlist: Vec<_> =
                cstr_playlist.iter().map(|track| track.as_ptr()).collect();

            ptr_playlist.push(null());

            unsafe {
                ga_set_playlist(
                    ptr_playlist.as_ptr() as *const *const c_char,
                    playlist.len() as u16,
                );
            }

            Ok(())
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn play() -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            unsafe {
                ga_play();
            }

            Ok(())
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn pause() -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            unsafe {
                ga_pause();
            }

            Ok(())
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn stop() -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            unsafe {
                ga_stop();
            }

            Ok(())
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn previous() -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            unsafe {
                ga_previous();
            }

            Ok(())
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn next() -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            unsafe {
                ga_next();
            }

            Ok(())
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn set_volume(volume: f32) -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            unsafe {
                ga_set_volume(volume);
            }

            Ok(())
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn get_volume() -> GrassAudioResult<f32> {
        if PLAYER_MUTEX.lock().is_ok() {
            Ok(unsafe { ga_get_volume() })
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn seek(position: f64) -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            unsafe {
                ga_seek(position);
            }

            Ok(())
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn skip_to(index: i16) -> GrassAudioResult<()> {
        if PLAYER_MUTEX.lock().is_ok() {
            unsafe {
                ga_skip_to(index);
            }

            Ok(())
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn get_playlist_index() -> GrassAudioResult<u16> {
        if PLAYER_MUTEX.lock().is_ok() {
            Ok(unsafe { ga_get_playlist_index() })
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn get_playlist_path() -> GrassAudioResult<Option<String>> {
        if PLAYER_MUTEX.lock().is_ok() {
            let path_ptr = unsafe { ga_get_playlist_path() };

            if path_ptr.is_null() {
                Ok(None)
            } else {
                let cstr = unsafe { CStr::from_ptr(path_ptr) };
                Ok(Some(
                    cstr.to_str()
                        .map_err(|_| GrassAudioError::InvalidPath)?
                        .to_string(),
                ))
            }
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn get_playlist_size() -> GrassAudioResult<u16> {
        if PLAYER_MUTEX.lock().is_ok() {
            Ok(unsafe { ga_get_playlist_size() })
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn get_playback_state() -> GrassAudioResult<PlaybackState> {
        if PLAYER_MUTEX.lock().is_ok() {
            let state;
            unsafe {
                state = ga_get_playback_state();
            }

            match state {
                GA_PLAYBACK_STATE_STOPPED => Ok(PlaybackState::Stopped),
                GA_PLAYBACK_STATE_PLAYING => Ok(PlaybackState::Playing),
                GA_PLAYBACK_STATE_PAUSED => Ok(PlaybackState::Paused),
                _ => Err(GrassAudioError::Unknown),
            }
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn get_position() -> GrassAudioResult<f64> {
        if PLAYER_MUTEX.lock().is_ok() {
            Ok(unsafe { ga_get_position() })
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }

    pub fn get_length() -> GrassAudioResult<f64> {
        if PLAYER_MUTEX.lock().is_ok() {
            Ok(unsafe { ga_get_length() })
        } else {
            Err(GrassAudioError::LockResourceFailed)
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn basic_playback() -> GrassAudioResult<()> {
        let manifest_dir_path = Path::new(env!("CARGO_MANIFEST_DIR"));
        let sample_files_path = manifest_dir_path.join("../../test/sample-files");

        GrassAudio::init(SampleRate::Hz44100).unwrap();
        assert_eq!(GrassAudio::get_playlist_path()?, None);

        let track1 = sample_files_path.join("01_Ghosts_I.flac");
        let track2 = sample_files_path.join("24_Ghosts_III.flac");
        let track3 = sample_files_path.join("25_Ghosts_III.flac");

        let tracks = vec![track1, track2, track3];

        GrassAudio::set_playlist(&tracks)?;

        GrassAudio::play()?;

        std::thread::sleep(std::time::Duration::from_secs(5));

        GrassAudio::pause()?;

        std::thread::sleep(std::time::Duration::from_secs(5));

        GrassAudio::skip_to(1)?;
        GrassAudio::play()?;

        assert_eq!(
            GrassAudio::get_playlist_path()?.unwrap(),
            tracks[1].to_str().unwrap()
        );

        std::thread::sleep(std::time::Duration::from_secs(5));

        GrassAudio::terminate().unwrap();

        Ok(())
    }
}
