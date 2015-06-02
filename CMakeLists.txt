cmake_minimum_required(VERSION 2.8.3)
project(rsim)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(rsim
    src/main.cpp
    src/world.cpp

    src/types.h
    src/math.h
    src/world.h
    src/object.h
    src/behavior.h
    src/update.h
)
