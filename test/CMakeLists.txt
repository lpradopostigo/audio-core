add_executable(test test.cpp)
target_link_libraries(test PUBLIC grass_audio)
add_custom_command(TARGET test POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/lib/bass/windows/x64/bass.dll
        $<TARGET_FILE_DIR:test>/bass.dll)

add_custom_command(TARGET test POST_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/lib/bass/windows/x64/bassmix.dll
        $<TARGET_FILE_DIR:test>/bassmix.dll)