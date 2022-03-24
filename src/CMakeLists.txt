add_library(grass_audio STATIC grass_audio.cpp)
target_include_directories(grass_audio PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(grass_audio PUBLIC bass bassmix bassflac log)