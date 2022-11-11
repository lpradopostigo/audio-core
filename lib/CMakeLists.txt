add_library(bass SHARED IMPORTED GLOBAL)
set_target_properties(bass
        PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bass/bass.dll"
        IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/bass/bass.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bass")


add_library(bassflac SHARED IMPORTED GLOBAL)
set_target_properties(bassflac
        PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bassflac/bassflac.dll"
        IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/bassflac/bassflac.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bassflac")

add_library(bassmix SHARED IMPORTED GLOBAL)
set_target_properties(bassmix
        PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/bassmix/bassmix.dll"
        IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/bassmix/bassmix.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bassmix")

