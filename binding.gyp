{
  "targets": [
    {
      "target_name": "grass_audio",
      "sources": [
        "cpp_src/binding.cpp",
        "cpp_src/grass_audio_wrapper.cpp",
        "cpp_src/grass_audio.cpp"
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
        "<(module_root_dir)/cpp_src/lib/bass/x64/bass.lib"
      ],
      "copies": [
        {
          "destination": "<(module_root_dir)/build/Release/",
          "files": [
            "<(module_root_dir)/cpp_src/lib/bass/x64/bass.dll"
          ]
        }
      ]
    }
  ]
}