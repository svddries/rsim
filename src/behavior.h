#ifndef RSIM_BEHAVIOR_H_
#define RSIM_BEHAVIOR_H_

class World;
class Update;

class Behavior
{

public:

    Behavior() {}

    virtual ~Behavior() {}

    virtual void update(const World& world, Update& u) {}

};

#endif
