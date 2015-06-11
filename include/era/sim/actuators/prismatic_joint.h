#ifndef RSIM_ACTUATORS_PRISMATIC_JOINT_H_
#define RSIM_ACTUATORS_PRISMATIC_JOINT_H_

#include "joint.h"

class PrismaticJoint : public Joint
{

public:

    PrismaticJoint(Id id) : Joint(id) {}

    void Update(const World& world, WorldUpdate& u)
    {
        if (!this->UpdatePos(world))
            return;

        Transform3 offset(Mat3::identity(), axis_ * position());
        u.setTransform(id_, origin_ * offset);
    }

};

#endif
