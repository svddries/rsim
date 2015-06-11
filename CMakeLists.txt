cmake_minimum_required(VERSION 2.8.3)
project(rsim)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    include
    3rdparty
)

# ---------------------------------------------------------------------------------------------

add_library(era_geometry
    include/era/geometry/mesh.h
    include/era/geometry/primitives.h
    src/geometry/primitives.cpp
)    

# ---------------------------------------------------------------------------------------------

add_library(era_io
    include/era/io/memory.h
    include/era/io/shared_memory.cpp
    include/era/io/shared_memory.h
    include/era/io/value.h
    include/era/io/vector.h
    include/era/io/package.h
)
target_link_libraries(era_io rt)

# ---------------------------------------------------------------------------------------------

add_library(era_sim
    src/sim/world.cpp

    include/era/sim/types.h
    include/era/sim/world.h
    include/era/sim/object.h
    include/era/sim/behavior.h
    include/era/sim/update.h

    include/era/sim/actuators/joint.h
    include/era/sim/actuators/rotational_joint.h
    include/era/sim/actuators/prismatic_joint.h

    include/era/sim/sensors/rasterizer.h
    src/sim/sensors/rasterizer.cpp
    include/era/sim/sensors/rgbd_camera.h
    src/sim/sensors/rgbd_camera.cpp

    include/era/sim/sensors/lrf_renderer.h
    src/sim/sensors/lrf_renderer.cpp
    include/era/sim/sensors/laser_range_finder.h
    src/sim/sensors/laser_range_finder.cpp
)

add_executable(rsim
    src/sim/main.cpp
)

target_link_libraries(rsim era_sim era_geometry era_io)

# ----------------------------------------------------------------------

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#add_executable(rsim_test_io
#    test/test_io.cpp
#)

add_executable(rsim_test_rgbd test/test_rgbd.cpp)
target_link_libraries(rsim_test_rgbd era_io ${OpenCV_LIBS})

add_executable(rsim_test_lrf test/test_lrf.cpp)
target_link_libraries(rsim_test_lrf era_io era_sim ${OpenCV_LIBS})
