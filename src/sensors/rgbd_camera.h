#ifndef RSIM_SENSORS_RGBD_CAMERA_H_
#define RSIM_SENSORS_RGBD_CAMERA_H_

#include "../behavior.h"
#include "../types.h"

#include "rasterizer.h"

#include <vector>

class RGBDCamera : public Behavior
{

public:

    RGBDCamera(Id id);

    ~RGBDCamera();

    void Update(const World& world, WorldUpdate& u);

private:

    Id id_;

    Rasterizer rasterizer_;

    std::vector<float> canvas_;

};

#endif
