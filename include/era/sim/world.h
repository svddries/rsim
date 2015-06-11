#ifndef RSIM_WORLD_H_
#define RSIM_WORLD_H_

#include "object.h"

#include <vector>
#include <queue>

class Behavior;

class World
{

public:

    World();

    ~World();

    Id AddObject(const Object& obj);

    void AddBehavior(Behavior* b);

    void SetObjectParent(Id child_id, Id parent_id, const Transform3& t);

    void Update(double dt);

    Time time() const { return time_; }

    Object& object(Id id) { return objects_[id]; }

    const Object& object(Id id) const { return objects_[id]; }

    const std::vector<Object>& objects() const { return objects_; }

    Id root() const { return root_; }

private:

    Time time_;

    Id root_;

    std::vector<Object> objects_;

    std::vector<Id> changed_object_ids_;

    bool object_graph_changed_;

    std::vector<Behavior*> behaviors_;

    // Tree datastructure for easier depth-first walk
    std::vector<std::pair<Id, uint64_t>> tree_;

    void MarkAsChanged(Id id);

};

#endif
