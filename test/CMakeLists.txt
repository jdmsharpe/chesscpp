cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

project(chess_tests)

enable_testing()

# add the executable
file(GLOB SOURCES
    *.cpp
    ../src/Pieces.cpp
    ../src/Board.cpp
)

include_directories(../inc)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(chess_tests ${SOURCES})

target_link_libraries(chess_tests ${GTEST_BOTH_LIBRARIES} pthread)
