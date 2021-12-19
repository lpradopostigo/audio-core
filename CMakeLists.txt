cmake_minimum_required(VERSION 3.21)
project(audio_player)

set(CMAKE_CXX_STANDARD 17)
include_directories(cpp_src/lib/bass/ node_modules/node-addon-api node_modules/node-api-headers/include)

add_library(bass STATIC IMPORTED)
add_library(audio_player_wrapper cpp_src/AudioPlayerWrapper.cpp cpp_src/AudioPlayerWrapper.h)

set_target_properties(
        bass
        PROPERTIES LINKER_LANGUAGE C
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/cpp_src/lib/bass/x64/bass.lib)


add_executable(audio_player cpp_src/main.cpp cpp_src/AudioPlayer.cpp cpp_src/AudioPlayer.h)
target_link_libraries(audio_player bass)
