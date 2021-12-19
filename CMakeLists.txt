cmake_minimum_required(VERSION 3.21)
project(grass_audio)

set(CMAKE_CXX_STANDARD 17)
include_directories(cpp_src/lib/bass/ node_modules/node-addon-api node_modules/node-api-headers/include)

add_library(bass STATIC IMPORTED)

add_library(grass_audio_wrapper cpp_src/GrassAudioWrapper.cpp cpp_src/GrassAudioWrapper.h)
add_library(binding cpp_src/binding.cpp)

set_target_properties(
        bass
        PROPERTIES LINKER_LANGUAGE C
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/cpp_src/lib/bass/x64/bass.lib)

add_executable(grass_audio cpp_src/test.cpp cpp_src/GrassAudio.cpp cpp_src/GrassAudio.h)
target_link_libraries(grass_audio bass)
