use grass_audio_sys::*;
use std::ffi::{CString};
use std::os::raw::c_char;
use std::path::Path;
use std::ptr::null;

pub enum SampleRate {
    Hz44100 = 44100,
    Hz48000 = 48000,
    Hz88200 = 88200,
    Hz96000 = 96000,
    Hz192000 = 192000,
}

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
        result = GA_Init(sample_rate as u32);
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
        result = GA_Terminate();
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
        GA_SetPlaylist(
            ptr_playlist.as_ptr() as *const *const c_char,
            playlist.len() as u16,
        );
    }
}

pub fn play() {
    unsafe {
        GA_Play();
    }
}

pub fn pause() {
    unsafe {
        GA_Pause();
    }
}

pub fn stop() {
    unsafe {
        GA_Stop();
    }
}

pub fn previous() {
    unsafe {
        GA_Previous();
    }
}

pub fn next() {
    unsafe {
        GA_Next();
    }
}

pub fn set_volume(volume: f32) {
    unsafe {
        GA_SetVolume(volume);
    }
}

pub fn get_volume() -> f32 {
    unsafe { GA_GetVolume() }
}

pub fn seek(position: f64) {
    unsafe {
        GA_Seek(position);
    }
}

pub fn skip_to_track(index: i16) {
    unsafe {
        GA_SkipToTrack(index);
    }
}

pub fn get_current_track_index() -> u16 {
    unsafe { GA_GetCurrentTrackIndex() }
}

pub fn get_playlist_size() -> u16 {
    unsafe { GA_GetPlaylistSize() }
}

pub fn get_playback_state() -> PlaybackState {
    let state;
    unsafe { state = GA_GetPlaybackState(); }

    match state {
        GA_PLAYBACK_STATE_STOPPED => PlaybackState::Stopped,
        GA_PLAYBACK_STATE_PLAYING => PlaybackState::Playing,
        GA_PLAYBACK_STATE_PAUSED => PlaybackState::Paused,
        _ => panic!("unknown playback state"),
    }
}

pub fn get_track_position() -> f64 {
    unsafe { GA_GetTrackPosition() }
}

pub fn get_track_length() -> f64 {
    unsafe { GA_GetTrackLength() }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn basic_playback() {
        let sample_files_path = Path::new(concat!(env!("CARGO_MANIFEST_DIR"), "../../../test/sample-files"));
        init(SampleRate::Hz44100).unwrap();

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

        std::thread::sleep(std::time::Duration::from_secs(5));


        terminate().unwrap();
    }
}
