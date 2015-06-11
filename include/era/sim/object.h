#ifndef RSIM_OBJECT_H_
#define RSIM_OBJECT_H_

#include "types.h"
#include "math.h"
#include "era/geometry/mesh.h"

struct Object
{
    Object() : parent_id(INVALID_ID), revision(0), changed(false) {}

    Id parent_id;
    std::vector<Id> children_ids;

    uint64_t tree_index;

    std::string name;
    Transform3 transform;
    Transform3 abs_transform;
    Mesh mesh;
    uint64_t revision;

    bool changed;

    bool has_parent() const { return parent_id != INVALID_ID; }
};

#endif
