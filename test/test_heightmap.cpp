#include <era/geometry/mesh.h>
#include <era/geometry/heightmap.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Please provide heightmap (png)" << std::endl;
        return 1;
    }

    era::Mesh mesh;
    era::loadHeightmap(argv[1], 0.01, 1, mesh);

    return 0;
}
