{
  "targets": [
    {
      "target_name": "grass_audio",
      "sources": [
        "src/binding.cpp",
        "src/grass_audio_wrapper.cpp",
        "src/grass_audio.cpp",
        "src/util.cpp",
        "lib/log/src/log.c"
      ],
      "conditions": [
        [
          "OS=='win'",
          {
            "msvs_settings": {
              "VCCLCompilerTool": {
                "AdditionalOptions": [
                  "-std:c++20"
                ]
              }
            }
          }
        ]
      ],
      "include_dirs": [
        "<!(node -p \"require('node-addon-api').include_dir\")",
        "lib/bass/",
        "lib/log/src/",
        "include/"
      ],
      "defines": [
        "NAPI_DISABLE_CPP_EXCEPTIONS"
      ],
      "libraries": [
        "<(module_root_dir)/lib/bass/windows/x64/bass.lib",
        "<(module_root_dir)/lib/bass/windows/x64/bassmix.lib",
        "<(module_root_dir)/lib/bass/windows/x64/bassflac.lib"
      ],
      "copies": [
        {
          "destination": "<(module_root_dir)/build/Release/",
          "files": [
            "<(module_root_dir)/lib/bass/windows/x64/bass.dll",
            "<(module_root_dir)/lib/bass/windows/x64/bassmix.dll",
            "<(module_root_dir)/lib/bass/windows/x64/bassflac.dll"
          ]
        }
      ]
    }
  ]
}