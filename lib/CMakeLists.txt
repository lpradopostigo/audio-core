add_library(log STATIC log/src/log.c)
target_include_directories(log PUBLIC log/src)

add_library(bass STATIC IMPORTED GLOBAL)
if (WIN32)
    set_target_properties(
            bass
            PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bass.dll"
            IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bass.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bass")
elseif (UNIX AND NOT APPLE)
    set_target_properties(
            bass
            PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bass/linux/x64/libbass.so"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bass")
endif ()

add_library(bassmix STATIC IMPORTED GLOBAL)
if (WIN32)
    set_target_properties(
            bassmix
            PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bassmix.dll"
            IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bassmix.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bass")
elseif (UNIX AND NOT APPLE)
    set_target_properties(
            bassmix
            PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bass/linux/x64/libbassmix.so"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bass")
endif ()


add_library(bassflac STATIC IMPORTED GLOBAL)
if (WIN32)
    set_target_properties(
            bassflac
            PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bassflac.dll"
            IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/bass/windows/x64/bassflac.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bass")
elseif (UNIX AND NOT APPLE)
    set_target_properties(
            bassflac
            PROPERTIES
            IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bass/linux/x64/libbassflac.so"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bass")
endif ()
