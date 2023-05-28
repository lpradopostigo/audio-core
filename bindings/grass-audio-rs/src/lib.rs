use grass_audio_sys::*;
use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use std::path::Path;
use std::ptr::null;

#[derive(Debug)]
pub enum SampleRate {
    Hz44100 = 44100,
    Hz48000 = 48000,
    Hz88200 = 88200,
    Hz96000 = 96000,
    Hz192000 = 192000,
}

#[derive(Debug)]
pub enum PlaybackState {
    Stopped,
    Playing,
    Paused,
}

#[derive(Debug)]
pub enum Error {
    InitFailed,
    TerminateFailed,
}

pub fn init(sample_rate: SampleRate) -> Result<(), Error> {
    let result;
    unsafe {
        result = ga_init(sample_rate as u32);
    }

    if result == GA_RESULT_OK {
        Ok(())
    } else {
        Err(Error::InitFailed)
    }
}

pub fn terminate() -> Result<(), Error> {
    let result;
    unsafe {
        result = ga_terminate();
    }

    if result == GA_RESULT_OK {
        Ok(())
    } else {
        Err(Error::TerminateFailed)
    }
}

pub fn set_playlist<T: AsRef<Path>>(playlist: &[T]) {
    //todo check path encoding possible problems

    let cstr_playlist: Vec<_> = playlist.iter()
        .map(|path| CString::new(path.as_ref().as_os_str().to_str().unwrap()).unwrap())
        .collect();

    let mut ptr_playlist: Vec<_> = cstr_playlist.iter()
        .map(|track| track.as_ptr())
        .collect();

    ptr_playlist.push(null());

    unsafe {
        ga_set_playlist(
            ptr_playlist.as_ptr() as *const *const c_char,
            playlist.len() as u16,
        );
    }
}

pub fn play() {
    unsafe {
        ga_play();
    }
}

pub fn pause() {
    unsafe {
        ga_pause();
    }
}

pub fn stop() {
    unsafe {
        ga_stop();
    }
}

pub fn previous() {
    unsafe {
        ga_previous();
    }
}

pub fn next() {
    unsafe {
        ga_next();
    }
}

pub fn set_volume(volume: f32) {
    unsafe {
        ga_set_volume(volume);
    }
}

pub fn get_volume() -> f32 {
    unsafe { ga_get_volume() }
}

pub fn seek(position: f64) {
    unsafe {
        ga_seek(position);
    }
}

pub fn skip_to_track(index: i16) {
    unsafe {
        ga_skip_to_track(index);
    }
}

pub fn get_current_track_index() -> u16 {
    unsafe { ga_get_current_track_index() }
}

pub fn get_current_track_path() -> Option<String> {
    let path_ptr = unsafe { ga_get_current_track_path() };

    if path_ptr.is_null() {
        None
    } else {
        let cstr = unsafe { CStr::from_ptr(path_ptr) };
        Some(cstr.to_str().unwrap().to_string())
    }
}

pub fn get_playlist_size() -> u16 {
    unsafe { ga_get_playlist_size() }
}

pub fn get_playback_state() -> PlaybackState {
    let state;
    unsafe { state = ga_get_playback_state(); }

    match state {
        GA_PLAYBACK_STATE_STOPPED => PlaybackState::Stopped,
        GA_PLAYBACK_STATE_PLAYING => PlaybackState::Playing,
        GA_PLAYBACK_STATE_PAUSED => PlaybackState::Paused,
        _ => panic!("unknown playback state"),
    }
}

pub fn get_track_position() -> f64 {
    unsafe { ga_get_track_position() }
}

pub fn get_track_length() -> f64 {
    unsafe { ga_get_track_length() }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn basic_playback() {
        let manifest_dir_path = Path::new(env!("CARGO_MANIFEST_DIR"));
        let sample_files_path = manifest_dir_path.join("../../test/sample-files");

        init(SampleRate::Hz44100).unwrap();
        assert_eq!(get_current_track_path(), None);

        let track1 = sample_files_path.join("01_Ghosts_I.flac");
        let track2 = sample_files_path.join("24_Ghosts_III.flac");
        let track3 = sample_files_path.join("25_Ghosts_III.flac");

        let tracks = vec![track1, track2, track3];

        set_playlist(&tracks);

        play();

        std::thread::sleep(std::time::Duration::from_secs(5));

        pause();

        std::thread::sleep(std::time::Duration::from_secs(5));

        skip_to_track(1);
        play();

        assert_eq!(get_current_track_path().unwrap(), tracks[1].to_str().unwrap());

        std::thread::sleep(std::time::Duration::from_secs(5));

        terminate().unwrap();
    }
}
