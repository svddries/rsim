#include <era/geometry/mesh.h>
#include <era/geometry/heightmap.h>

// Visualization
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Please provide heightmap (png)" << std::endl;
        return 1;
    }

    std::string image_filename = argv[1];

    era::Mesh mesh;
    era::loadHeightmap(image_filename, 1, 1, mesh);

    // Read input image
    cv::Mat viz = cv::imread(image_filename);

    if (!viz.data)
    {
        std::cout << "Could not load file." << std::endl;
        return 1;
    }

    // Display number of vertices and triangles
    std::cout << mesh.triangles.size() << " vertices" << std::endl;
    std::cout << mesh.vertices.size() << " triangles" << std::endl;

    // Visualize triangles
    for(const era::Triangle& t : mesh.triangles)
    {
        era::Vec3 v1 = mesh.vertices[t.i1];
        era::Vec3 v2 = mesh.vertices[t.i2];
        era::Vec3 v3 = mesh.vertices[t.i3];

        v1.y = viz.rows - v1.y;
        v2.y = viz.rows - v2.y;
        v3.y = viz.rows - v3.y;

        cv::line(viz, cv::Point(v1.x, v1.y), cv::Point(v2.x, v2.y), cv::Scalar(0, 0, 255), 1);
        cv::line(viz, cv::Point(v2.x, v2.y), cv::Point(v3.x, v3.y), cv::Scalar(0, 0, 255), 1);
        cv::line(viz, cv::Point(v3.x, v3.y), cv::Point(v1.x, v1.y), cv::Scalar(0, 0, 255), 1);
    }

    cv::imshow("image", viz);
    cv::waitKey();

    return 0;
}
