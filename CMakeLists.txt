cmake_minimum_required(VERSION 3.21)

project(grass_audio
        LANGUAGES C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(DIST_DIR ${CMAKE_SOURCE_DIR}/dist)

if (MSVC)
    add_compile_options(/W4 /WX)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif ()

add_subdirectory(lib)
add_subdirectory(test)
add_library(grass_audio SHARED src/grass_audio.c src/utils.c)
target_include_directories(grass_audio PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(grass_audio
        PUBLIC bass
        PRIVATE bassmix
        PRIVATE bassflac)
add_custom_command(TARGET grass_audio POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:grass_audio> $<TARGET_FILE_DIR:grass_audio>
        COMMAND_EXPAND_LISTS)

install(TARGETS grass_audio
        DESTINATION ${DIST_DIR})

install(TARGETS grass_audio
        DESTINATION ${DIST_DIR}/../bindings/rust/grass-audio)

install(FILES $<TARGET_RUNTIME_DLLS:grass_audio> include/grass_audio.h DESTINATION ${DIST_DIR})
install(FILES $<TARGET_RUNTIME_DLLS:grass_audio> include/grass_audio.h DESTINATION ${DIST_DIR}/../bindings/rust/grass-audio)
