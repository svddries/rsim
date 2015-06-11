#ifndef RSIM_ACTUATORS_ROTATIONAL_JOINT_H_
#define RSIM_ACTUATORS_ROTATIONAL_JOINT_H_

#include "joint.h"

namespace era
{
namespace sim
{

class RotationalJoint : public Joint
{

public:

    RotationalJoint(Id id) : Joint(id) {}

    void Update(const World& world, WorldUpdate& u)
    {
        if (!this->UpdatePos(world))
            return;

        double s = sin(position());
        double c = cos(position());

        float& ux = axis_.x;
        float& uy = axis_.y;
        float& uz = axis_.z;

        Transform3 offset;
        offset.R.xx = c + ux * ux * (1 - c);
        offset.R.xy = ux * uy * (1 - c) - uz * s;
        offset.R.xz = ux * uz * (1 - c) + uy * s;

        offset.R.yx = uy * ux * (1 - c) + uz * s;
        offset.R.yy = c + uy * uy * (1 - c);
        offset.R.yz = uy * uz * (1 - c) - ux * s;

        offset.R.zx = uz * ux * (1 - c) - uy * s;
        offset.R.zy = uz * uy * (1 - c) + ux * s;
        offset.R.zz = c + uz * uz * (1 - c);

        offset.t = Vec3(0, 0, 0);

        u.setTransform(id_, origin_ * offset);
    }

};

}
}

#endif
