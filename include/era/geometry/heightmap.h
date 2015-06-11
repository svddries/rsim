#ifndef ERA_GEOMETRY_HEIGHTMAP_H_
#define ERA_GEOMETRY_HEIGHTMAP_H_

#include "era/geometry/math.h"

namespace era
{

struct Mesh;

bool loadHeightmap(const std::string filename, double resolution, double blockheight, Mesh& mesh);

}

#endif
