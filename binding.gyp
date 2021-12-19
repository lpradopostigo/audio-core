{
  "targets": [
    {
      "target_name": "grass_audio",
      "sources": [
        "cpp_src/binding.cpp",
        "cpp_src/GrassAudioWrapper.cpp",
        "cpp_src/GrassAudio.cpp"
      ],
      "conditions": [
        [
          "OS=='win'",
          {
            "msvs_settings": {
              "VCCLCompilerTool": {
                "AdditionalOptions": [
                  "-std:c++17"
                ]
              }
            }
          }
        ],
        [
          "OS=='mac'",
          {
            "xcode_settings": {
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
              "CLANG_CXX_LIBRARY": "libc++",
              "MACOSX_DEPLOYMENT_TARGET": "10.7"
            }
          }
        ]
      ],
      "include_dirs": [
        "<!(node -p \"require('node-addon-api').include_dir\")",
        "cpp_src/lib/bass/"
      ],
      "defines": [
        "NAPI_DISABLE_CPP_EXCEPTIONS"
      ],
      "libraries": [
        "<(module_root_dir)/cpp_src/lib/bass/x64/bass"
      ]
    }
  ]
}