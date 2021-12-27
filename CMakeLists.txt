cmake_minimum_required(VERSION 3.21)
project(grass_audio)

set(CMAKE_CXX_STANDARD 20)
include_directories(cpp_src/lib/bass/ node_modules/node-addon-api node_modules/node-api-headers/include)

add_library(bass STATIC IMPORTED)

add_library(grass_audio_wrapper cpp_src/grass_audio_wrapper.cpp cpp_src/grass_audio_wrapper.h)
add_library(binding cpp_src/binding.cpp)
add_library(callable_to_pointer cpp_src/callable_to_pointer.hpp)

set_target_properties(
        callable_to_pointer
        PROPERTIES LINKER_LANGUAGE CXX)

set_target_properties(
        bass
        PROPERTIES LINKER_LANGUAGE C
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/cpp_src/lib/bass/x64/bass.lib)

add_executable(grass_audio cpp_src/test.cpp cpp_src/grass_audio.cpp cpp_src/grass_audio.h)
target_link_libraries(grass_audio bass)
