#ifndef RSIM_BEHAVIOR_H_
#define RSIM_BEHAVIOR_H_

class World;
class WorldUpdate;

class Behavior
{

public:

    Behavior() {}

    virtual ~Behavior() {}

    virtual void Update(const World& world, WorldUpdate& u) {}

};

#endif
