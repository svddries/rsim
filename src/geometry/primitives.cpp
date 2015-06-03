#include "mesh.h"

void createBox(const Vec3f& min, const Vec3f& max, Mesh& mesh)
{
    int p0 = mesh.addPoint(Vec3f(min.x, min.y, min.z)); // 0
    int p1 = mesh.addPoint(Vec3f(max.x, min.y, min.z)); // 1
    int p2 = mesh.addPoint(Vec3f(min.x, max.y, min.z)); // 2
    int p3 = mesh.addPoint(Vec3f(max.x, max.y, min.z)); // 3
    int p4 = mesh.addPoint(Vec3f(min.x, min.y, max.z)); // 4
    int p5 = mesh.addPoint(Vec3f(max.x, min.y, max.z)); // 5
    int p6 = mesh.addPoint(Vec3f(min.x, max.y, max.z)); // 6
    int p7 = mesh.addPoint(Vec3f(max.x, max.y, max.z)); // 7

    // back plane
    mesh.triangles.push_back(Triangle(p1, p0, p2));
    mesh.triangles.push_back(Triangle(p1, p2, p3));

    // front plane
    mesh.triangles.push_back(Triangle(p4, p5, p6));
    mesh.triangles.push_back(Triangle(p6, p5, p7));

    // left plane
    mesh.triangles.push_back(Triangle(p0, p4, p2));
    mesh.triangles.push_back(Triangle(p2, p4, p6));

    // right plane
    mesh.triangles.push_back(Triangle(p5, p1, p3));
    mesh.triangles.push_back(Triangle(p5, p3, p7));

    // top plane
    mesh.triangles.push_back(Triangle(p0, p1, p4));
    mesh.triangles.push_back(Triangle(p4, p1, p5));

    // bottom plane
    mesh.triangles.push_back(Triangle(p3, p2, p6));
    mesh.triangles.push_back(Triangle(p3, p6, p7));
}
