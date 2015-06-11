#ifndef RSIM_SENSORS_LASER_RANGE_FINDER_H_
#define RSIM_SENSORS_LASER_RANGE_FINDER_H_

#include "../behavior.h"
#include "../types.h"

#include "era/io/package.h"
#include "era/io/value.h"
#include "era/io/vector.h"
#include "era/io/shared_memory.h"

#include "lrf_renderer.h"

class LaserRangeFinder : public Behavior
{

public:

    LaserRangeFinder(Id id);

    ~LaserRangeFinder();

    void Update(const World& world, WorldUpdate& u);

private:

    Id id_;

    LRFRenderer renderer_;

    // Data

    io::SharedMemory mem_;

    io::Package scan_pkg_;
    io::Value<float> angle_min_;
    io::Value<float> angle_max_;
    io::Vector<float> ranges_;

};

#endif
