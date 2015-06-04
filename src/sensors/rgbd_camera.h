#ifndef RSIM_SENSORS_RGBD_CAMERA_H_
#define RSIM_SENSORS_RGBD_CAMERA_H_

#include "../behavior.h"
#include "../types.h"

#include "../io/package.h"
#include "../io/value.h"
#include "../io/vector.h"
#include "../io/shared_memory.h"

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
