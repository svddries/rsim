#ifndef ERA_GEOMETRY_PRIMITIVES_H_
#define ERA_GEOMETRY_PRIMITIVES_H_

#include "../math.h"

namespace era
{

class Mesh;

void createBox(const Vec3f& min, const Vec3f& max, Mesh& mesh);

}

#endif
