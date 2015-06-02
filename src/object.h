#ifndef RSIM_OBJECT_H_
#define RSIM_OBJECT_H_

#include "types.h"
#include "math.h"

#include <era/geometry/mesh.h>

struct Object
{
    Object() : parent_id(0), revision(0) {}

    Id parent_id;
    Transform3 transform;
    era::geometry::Mesh mesh;
    uint64_t revision;
};

#endif
