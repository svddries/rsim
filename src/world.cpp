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

World::World() : time_(0), object_list_changed_(false)
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
      changed_object_ids_.push_back(child_id);
      c.changed = true;

      Object& p = objects_[parent_id];
      p.children_ids.push_back(child_id);

      object_list_changed_ = true;
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

void World::update(double dt)
{
    if (object_list_changed_)
    {
        // Re-build tree object (depth-first)
        tree_.clear();
        tree_.push_back(std::make_pair(root_, 0));
        objects_[root_].tree_index = 0;;

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

//        for(auto p : tree_)
//            std::cout << "(" << p.first << ", " << p.second << ") ";
//        std::cout << std::endl;

        object_list_changed_ = false;
    }

    std::cout << "Num changed: " << changed_object_ids_.size() << std::endl;

    std::sort(changed_object_ids_.begin(), changed_object_ids_.end(), DepthFirstOrdering(objects_));

    for(Id id : changed_object_ids_)
    {
        Object& obj = objects_[id];
        if (!obj.changed)
            continue;

        std::cout << "    " << obj.name << " " << tree_[obj.tree_index].second << std::endl;

        for(unsigned int i = obj.tree_index; i < obj.tree_index + tree_[obj.tree_index].second; ++i)
        {
            Object& o = objects_[tree_[i].first];
            o.abs_transform = objects_[o.parent_id].abs_transform * o.transform;
            std::cout << o.name << ": " << o.abs_transform << std::endl;
            o.changed = false;
        }
    }

    changed_object_ids_.clear();

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

        std::cout << "Setting " << u.first << " to " << o.transform << std::endl;

        ++o.revision;

        o.changed = true;
        changed_object_ids_.push_back(u.first);
    }
}
