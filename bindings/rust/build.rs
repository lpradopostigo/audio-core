extern crate bindgen;

use std::env;
use std::fs::copy;
use std::path::PathBuf;
use bindgen::CargoCallbacks;

fn main() {
    let grass_audio_dist_path = PathBuf::from("grass-audio")
        .canonicalize()
        .expect("cannot canonicalize path");

    let headers_path = grass_audio_dist_path.join("grass_audio.h");
    let headers_path_str = headers_path.to_str().expect("Path is not a valid string");

    let bindings = bindgen::Builder::default()
        .header(headers_path_str)
        .prepend_enum_name(false)
        .parse_callbacks(Box::new(CargoCallbacks))
        .generate()
        .expect("Unable to generate bindings");

    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());

    let bindings_path = out_dir.join("bindings.rs");
    bindings
        .write_to_file(bindings_path)
        .expect("Couldn't write bindings!");


    for entry in grass_audio_dist_path.read_dir().expect("cannot read directory") {
        let entry = entry.expect("cannot read entry");
        let path = entry.path();
        if path.is_file() {
            let file_name = path.file_name().expect("cannot get file name");
            let file_name_str = file_name.to_str().expect("cannot convert file name to string");
            if file_name_str.ends_with(".dll") || file_name_str.ends_with(".lib") {
                let dest_path = out_dir.join(file_name);
                copy(path.clone(), dest_path).expect("cannot copy file");
            }
        }
    }

    println!("cargo:rustc-link-search=native={}", out_dir.display());
    println!("cargo:rustc-link-lib=dylib=grass_audio");
}
