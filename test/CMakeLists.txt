add_compile_definitions(
        PROJECT_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        PROJECT_TEST_SAMPLE_FILES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/sample-files")

add_executable(test test.c)
target_link_libraries(test PUBLIC grass_audio)
add_custom_command(TARGET test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:test> $<TARGET_FILE_DIR:test>
        COMMAND_EXPAND_LISTS)

