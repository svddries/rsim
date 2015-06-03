cmake_minimum_required(VERSION 2.8.3)
project(rsim)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    include
    3rdparty
)

add_executable(rsim
    src/main.cpp
    src/world.cpp

    src/types.h
    src/math.h
    src/world.h
    src/object.h
    src/behavior.h
    src/update.h

    src/geometry/mesh.h
    src/geometry/primitives.h
    src/geometry/primitives.cpp

    src/actuators/joint.h
    src/actuators/rotational_joint.h
    src/actuators/prismatic_joint.h

    src/sensors/rasterizer.h
    src/sensors/rasterizer.cpp
    src/sensors/rgbd_camera.h
    src/sensors/rgbd_camera.cpp
)

find_package(OpenCV REQUIRED)
include_directories(
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(rsim  ${OpenCV_LIBS})
