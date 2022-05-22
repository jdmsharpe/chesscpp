cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# set the project name
project(chess)

# add the executable
file(GLOB SOURCES
    src/*.cpp
)

# Add FEN file
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/inc/load.fen DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Find and include SDL2 packages
include(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

include_directories(inc ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

add_executable(chess ${SOURCES})
target_link_libraries(chess ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
