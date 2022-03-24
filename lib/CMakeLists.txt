add_library(log STATIC log/src/log.c)
target_include_directories(log PUBLIC log/src)

add_library(bass STATIC IMPORTED GLOBAL)
set_target_properties(
        bass
        PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bass.dll"
        IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bass.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bass")

add_library(bassmix STATIC IMPORTED GLOBAL)
set_target_properties(
        bassmix
        PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bassmix.dll"
        IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bassmix.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bass")

add_library(bassflac STATIC IMPORTED GLOBAL)
set_target_properties(
        bassflac
        PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bassflac.dll"
        IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bassflac.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bass")