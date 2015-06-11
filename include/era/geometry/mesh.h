#ifndef ERA_GEOMETRY_MESH_H_
#define ERA_GEOMETRY_MESH_H_

#include "era/geometry/math.h"
#include <vector>

struct Triangle
{
    Triangle(unsigned int i1_, unsigned int i2_, unsigned int i3_)
        : i1(i1_), i2(i2_), i3(i3_) {}

    union {
        struct { unsigned int i1, i2, i3; };
        unsigned int i[3];
    };
};

struct Mesh
{
    std::vector<Vec3> vertices;
    std::vector<Triangle> triangles;

    inline unsigned int addPoint(const Vec3f& p)
    {
        vertices.push_back(p);
        return vertices.size() - 1;
    }
};

#endif
