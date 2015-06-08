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

    src/sensors/lrf_renderer.h
    src/sensors/lrf_renderer.cpp
    src/sensors/laser_range_finder.h
    src/sensors/laser_range_finder.cpp


    src/io/memory.h
    src/io/shared_memory.cpp
    src/io/shared_memory.h
    src/io/value.h
    src/io/vector.h
    src/io/package.h
)

find_package(OpenCV REQUIRED)
include_directories(
    ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(rsim rt ${OpenCV_LIBS})

# ----------------------------------------------------------------------

add_executable(rsim_test_io
    test/test_io.cpp
)

add_executable(rsim_test_rgbd
    test/test_rgbd.cpp
    src/io/shared_memory.cpp
)
target_link_libraries(rsim_test_rgbd rt ${OpenCV_LIBS})

add_executable(rsim_test_lrf
    test/test_lrf.cpp
    src/io/shared_memory.cpp
    src/sensors/lrf_renderer.cpp
)
target_link_libraries(rsim_test_lrf rt ${OpenCV_LIBS})
