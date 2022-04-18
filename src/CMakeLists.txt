add_library(grass_audio STATIC grass_audio.cpp)
target_include_directories(grass_audio PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(grass_audio PUBLIC bass bassmix bassflac log util)


add_library(grass_audio_wrapper STATIC grass_audio_wrapper.cpp)
target_include_directories(grass_audio PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/node_modules/node-addon-api"
        "${CMAKE_SOURCE_DIR}/node_modules/node-api-headers/include")
target_link_libraries(grass_audio_wrapper PUBLIC grass_audio)

add_library(binding binding.cpp)
target_link_libraries(binding PUBLIC grass_audio_wrapper)

add_library(util STATIC util.cpp)
target_include_directories(util PUBLIC
        "${CMAKE_SOURCE_DIR}/include")
