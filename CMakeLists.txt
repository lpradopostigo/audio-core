cmake_minimum_required(VERSION 3.21)

project(grass_audio
        VERSION 0.1
        LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
include_directories(
        node_modules/node-addon-api
        node_modules/node-api-headers/include)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)

add_executable(main test/main.cpp)
target_link_libraries(main PUBLIC log bass bassmix)
add_custom_command(TARGET main POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/lib/bass/windows/x64/bass.dll
        ${PROJECT_BINARY_DIR}/bass.dll)

add_custom_command(TARGET main POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/lib/bass/windows/x64/bassmix.dll
        ${PROJECT_BINARY_DIR}/bassmix.dll)


add_library(callable_to_pointer include/callable_to_pointer.hpp)
set_target_properties(
        callable_to_pointer
        PROPERTIES LINKER_LANGUAGE CXX)

