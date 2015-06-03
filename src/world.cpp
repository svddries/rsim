#include "world.h"
#include "behavior.h"
#include "update.h"

#include <stack>
#include <algorithm>

// ----------------------------------------------------------------------------------------------------

struct DepthFirstOrdering
{
    DepthFirstOrdering(const std::vector<Object>& objects_) : objects(objects_) {}

    bool operator()(Id id1, Id id2) const
    {
        return objects[id1].tree_index < objects[id2].tree_index;
    }

    const std::vector<Object>& objects;
};

// ----------------------------------------------------------------------------------------------------

World::World() : time_(0), object_graph_changed_(false)
{
    root_ = AddObject(Object());
    objects_[root_].abs_transform = Transform3::identity();
}

// ----------------------------------------------------------------------------------------------------

World::~World()
{
}

// ----------------------------------------------------------------------------------------------------

Id World::AddObject(const Object& obj)
{
    Id id = objects_.size();
    objects_.push_back(obj);
    return id;
}

// ----------------------------------------------------------------------------------------------------

void World::AddBehavior(Behavior* b)
{
    behaviors_.push_back(b);
}

// ----------------------------------------------------------------------------------------------------

void World::SetObjectParent(Id child_id, Id parent_id, const Transform3& t)
{
    Object& c = objects_[child_id];
    c.parent_id = parent_id;
    c.transform = t;

    Object& p = objects_[parent_id];
    p.children_ids.push_back(child_id);

    if (parent_id == root_)
    {
        object_graph_changed_ = true;
        MarkAsChanged(child_id);
    }
}

// ----------------------------------------------------------------------------------------------------

struct BlaNode
{
    BlaNode(Id id_, uint64_t tree_index_) : id(id_), i_child(0), tree_index(tree_index_) {}

    Id id;
    uint64_t i_child;
    uint64_t tree_index;
};

// ----------------------------------------------------------------------------------------------------

void World::Update(double dt)
{
    if (object_graph_changed_)
    {
        // Re-build tree object (depth-first)
        tree_.clear();
        tree_.push_back(std::make_pair(root_, 0));
        objects_[root_].tree_index = 0;

        std::stack<BlaNode> Q;
        Q.push(BlaNode(root_, tree_.size() - 1));


        while(!Q.empty())
        {
            BlaNode& n = Q.top();
            Object& obj = objects_[n.id];

            if (n.i_child < obj.children_ids.size())
            {
                Id child_id = obj.children_ids[n.i_child++];
                objects_[child_id].tree_index = tree_.size();
                tree_.push_back(std::make_pair(child_id, 0));
                Q.push(BlaNode(child_id, tree_.size() - 1));

            }
            else
            {
                uint64_t num_children = tree_[n.tree_index].second;
                Q.pop();

                if (!Q.empty())
                    tree_[Q.top().tree_index].second += num_children + 1;
            }
        }

        for(auto p : tree_)
            std::cout << "(" << p.first << ", " << p.second << ") ";
        std::cout << std::endl;

        object_graph_changed_ = false;
    }

    if (!changed_object_ids_.empty())
    {
        // Sort changed objects in depth first order
        std::sort(changed_object_ids_.begin(), changed_object_ids_.end(), DepthFirstOrdering(objects_));

        for(Id id : changed_object_ids_)
        {
            Object& obj = objects_[id];
            if (!obj.changed)
                continue;

            for(unsigned int i = obj.tree_index; i <= obj.tree_index + tree_[obj.tree_index].second; ++i)
            {
                Object& o = objects_[tree_[i].first];
                o.abs_transform = objects_[o.parent_id].abs_transform * o.transform;
                o.changed = false;
            }
        }

        changed_object_ids_.clear();
    }

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

        // Mark object as changed
        MarkAsChanged(u.first);
    }
}

// ----------------------------------------------------------------------------------------------------

void World::MarkAsChanged(Id id)
{
    Object& o = objects_[id];
    if (o.changed)
        return;

    o.changed = true;
    changed_object_ids_.push_back(id);
}
