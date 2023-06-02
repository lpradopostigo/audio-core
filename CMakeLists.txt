cmake_minimum_required(VERSION 3.21)

project(grass_player
        VERSION 0.1.0
        LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(DIST_DIR ${CMAKE_SOURCE_DIR}/dist)

if (MSVC)
    add_compile_options(/W4)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

add_subdirectory(lib)
add_subdirectory(test)
add_library(grass_player SHARED src/gp_audio_output.c src/gp_player.c src/gp_source.c src/gp_source_list.c)
target_include_directories(grass_player PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(grass_player
        PUBLIC bass
        PRIVATE bassmix
        PRIVATE bassflac)
add_custom_command(TARGET grass_player POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:grass_player> $<TARGET_FILE_DIR:grass_player>
        COMMAND_EXPAND_LISTS)

install(TARGETS grass_player
        DESTINATION ${DIST_DIR})

install(TARGETS grass_player
        DESTINATION ${DIST_DIR}/../bindings/grass-audio-rs/grass-audio-sys/dist)

install(FILES $<TARGET_RUNTIME_DLLS:grass_player> DESTINATION ${DIST_DIR})
install(FILES $<TARGET_RUNTIME_DLLS:grass_player> DESTINATION ${DIST_DIR}/../bindings/grass-audio-rs/grass-audio-sys/dist)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${DIST_DIR}/include)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${DIST_DIR}/../bindings/grass-audio-rs/grass-audio-sys/dist/include)
