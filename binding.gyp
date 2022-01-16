{
  "targets": [
    {
      "target_name": "grass_audio",
      "sources": [
        "cpp_src/binding.cpp",
        "cpp_src/GrassAudioWrapper.cpp",
        "cpp_src/Grass_Audio.h"
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
        "cpp_src/lib/bass/"
      ],
      "defines": [
        "NAPI_DISABLE_CPP_EXCEPTIONS"
      ],
      "libraries": [
        "<(module_root_dir)/cpp_src/lib/bass/x64/bass.lib",
        "<(module_root_dir)/cpp_src/lib/bass/x64/bassmix.lib"

      ],
      "copies": [
        {
          "destination": "<(module_root_dir)/build/Release/",
          "files": [
            "<(module_root_dir)/cpp_src/lib/bass/x64/bass.dll",
            "<(module_root_dir)/cpp_src/lib/bass/x64/bassmix.dll"
          ]
        }
      ]
    }
  ]
}