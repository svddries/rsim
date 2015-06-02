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

        std::cout << "update" << std::endl;

        Transform3 t;
        t.t = Vec3(0, 0, position());
        t.R.setRPY(0, 0, 0);

        u.setTransform(id_, t);
    }

};

#endif
