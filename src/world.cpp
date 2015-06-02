#include "world.h"
#include "behavior.h"
#include "update.h"

// ----------------------------------------------------------------------------------------------------

World::World() : time_(0)
{
}

// ----------------------------------------------------------------------------------------------------

World::~World()
{
}

// ----------------------------------------------------------------------------------------------------

Id World::AddObject(const Object& obj, const Transform3& t, Id parent)
{
    Id id = objects_.size();
    objects_.push_back(obj);

    Object& o = objects_.back();
    o.transform = t;
    o.parent_id = parent;

    return id;
}

// ----------------------------------------------------------------------------------------------------

void World::AddBehavior(Behavior* b)
{
    behaviors_.push_back(b);
}

// ----------------------------------------------------------------------------------------------------

void World::update(double dt)
{
    time_ += dt;

    WorldUpdate up;
    for(Behavior* b : behaviors_)
    {
        b->Update(*this, up);
    }

    // Update transforms
    for(auto u : up.transforms)
    {
        Object& o = objects_[u.first];
        o.transform = u.second;
        ++o.revision;
    }
}
