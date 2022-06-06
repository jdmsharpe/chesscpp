cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

project(chess_tests)

enable_testing()

# add the executable
file(GLOB SOURCES
    *.cpp
    ../src/Pieces.cpp
    ../src/Board.cpp
    ../src/Game.cpp
)

include_directories(../inc)

# Find and include SDL2 packages
include(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

include_directories(inc ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(chess_tests ${SOURCES})

target_link_libraries(chess_tests ${GTEST_BOTH_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} pthread)
