cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# set the project name
project(chess)

# add the executable
file(GLOB SOURCES
    inc/*.h
    src/*.cpp
)

# Add FEN file
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/inc/load.fen DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories(inc)

add_executable(chess ${SOURCES})
target_link_libraries(chess pthread)
