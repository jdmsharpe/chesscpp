cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

# set the project name
project(chess)

# add the executable
file(GLOB SOURCES
    inc/*.h
    src/*.cpp
)

include_directories(inc)

add_executable(chess ${SOURCES})
target_link_libraries(chess pthread)
