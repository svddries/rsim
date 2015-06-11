#ifndef RSIM_SENSORS_RGBD_CAMERA_H_
#define RSIM_SENSORS_RGBD_CAMERA_H_

#include "../behavior.h"
#include "../types.h"

#include "era/io/package.h"
#include "era/io/value.h"
#include "era/io/vector.h"
#include "era/io/shared_memory.h"

#include "rasterizer.h"

class RGBDCamera : public Behavior
{

public:

    RGBDCamera(Id id);

    ~RGBDCamera();

    void Update(const World& world, WorldUpdate& u);

private:

    Id id_;

    Rasterizer rasterizer_;

    // Data

    io::SharedMemory mem_;

    io::Package image_pkg_;
    io::Value<uint32_t> width_;
    io::Value<uint32_t> height_;
    io::Vector<float> canvas_;

};

#endif
