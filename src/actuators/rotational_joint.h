#ifndef RSIM_WORLD_H_
#define RSIM_WORLD_H_

#include "object.h"

#include <vector>

class Behavior;

class World
{

public:

    World();

    ~World();

    Id AddObject(const Object& obj, const Transform3& t, Id parent = 0);

    void AddBehavior(Behavior* b);

    void update(double dt);

    Time time() const { return time_; }

    const Object& object(Id id) const { return objects_[id]; }

private:

    std::vector<Object> objects_;

    std::vector<Behavior*> behaviors_;

    Time time_;

};

#endif
