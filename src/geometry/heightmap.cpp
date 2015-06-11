#include "era/geometry/heightmap.h"
#include "era/geometry/mesh.h"

#include "png++/png.hpp"

#include "polypartition.h"

namespace era
{

// ----------------------------------------------------------------------------------------------------

template<typename T>
class Grid
{

public:

    Grid(unsigned int width, unsigned int height) : width_(width), height_(height), data_(width * height) {}

    Grid(unsigned int width, unsigned int height, const T& v) : width_(width), height_(height), data_(width * height, v) {}

    inline T& operator[](unsigned int i) { return data_[i]; }

    inline const T& operator[](unsigned int i) const { return data_[i]; }

    inline T& operator()(unsigned int x, int y) { return data_[y * height_ + x]; }

    inline const T& operator()(unsigned int x, int y) const { return data_[y * height_ + x]; }

    inline unsigned int width() const { return width_; }

    inline unsigned int height() const { return height_; }

private:

    unsigned int width_, height_;
    std::vector<T> data_;
};

// ----------------------------------------------------------------------------------------------------

void findContours(const Grid<unsigned char>& heightmap, const Vec2i& p, int d_start, std::vector<Vec2i>& points,
                  std::vector<Vec2i>& line_starts, Grid<unsigned char>& contour_map, bool add_first)
{
    static int dx[4] = {1,  0, -1,  0 };
    static int dy[4] = {0,  1,  0, -1 };

    unsigned char v = heightmap(p.x, p.y);

    int d_current = d_start; // Current direction
    int x2 = p.x;
    int y2 = p.y;

    int line_piece_min = 1e9; // minimum line piece length of current line
    int line_piece_max = 0; // maximum line piece length of current line

    int d_main = d_current; // The main direction in which we're heading. If we follow a line
                            // that gradually changes to the side (1-cell side steps), this direction
                            // denotes the principle axis of the line

    if (add_first)
        points.push_back(p - Vec2i(1, 1));

    int n_uninterrupted = 1;
    Vec2i p_corner = p;

    while (true)
    {
        bool found = false;
        int d = (d_current + 3) % 4; // check going left first

        for(int i = -1; i < 3; ++i)
        {
            if (heightmap(x2 + dx[d], y2 + dy[d]) == v)
            {
                found = true;
                break;
            }

            d = (d + 1) % 4;
        }

        if (!found)
            return;

        Vec2i p_current(x2, y2);

        if ((d + 2) % 4 == d_current)
        {
            // 180 degree turn

            if (x2 == p.x && y2 == p.y) // Edge case: if we returned to the start point and
                                        // this is a 180 degree angle, return without adding it
                return;


            Vec2i q = p_current;
            if (d == 0 || d_current == 0) // right
                --q.y;
            if (d == 3 || d_current == 3) // up
                --q.x;

            points.push_back(q);
            d_main = d;
            line_piece_min = 1e9;
            line_piece_max = 0;

        }
        else if (d_current != d_main)
        {
            // Not moving in main direction (side step)

            if (d != d_main)
            {
                // We are not moving back in the main direction
                // Add the corner to the list and make this our main direction

                points.push_back(p_corner);
                d_main = d_current;
                line_piece_min = 1e9;
                line_piece_max = 0;
            }
        }
        else
        {
            // Moving in main direction (no side step)

            if (d_current != d)
            {
                // Turning 90 degrees

                // Check if the length of the last line piece is OK w.r.t. the other pieces in this line. If it differs to much,
                // (i.e., the contour has taken a different angle), add the last corner to the list. This way, we introduce a
                // bend in the contour
                if (line_piece_max > 0 && (n_uninterrupted < line_piece_max - 2 || n_uninterrupted > line_piece_min + 2))
                {
                    // Line is broken, add the corner as bend
                    points.push_back(p_corner);

                    line_piece_min = 1e9;
                    line_piece_max = 0;
                }

                // Update the line piece lenth boundaries with the current found piece
                line_piece_min = std::min(line_piece_min, n_uninterrupted);
                line_piece_max = std::max(line_piece_max, n_uninterrupted);
            }
        }

        if (d_current != d)
        {
            Vec2i q = p_current;
            if (d == 0 || d_current == 0) // right
                --q.y;
            if (d == 3 || d_current == 3) // up
                --q.x;

            p_corner = q;
            n_uninterrupted = 0;
        }

        if ((d_current == 3 && d != 2) || (d == 3 && d != 0)) // up
            line_starts.push_back(p_current);

        contour_map(p_current.x, p_current.y) = 1;

        ++n_uninterrupted;

        if (points.size() > 1 && x2 == p.x && y2 == p.y)
            return;

        x2 = x2 + dx[d];
        y2 = y2 + dy[d];

        d_current = d;
    }
}

// ----------------------------------------------------------------------------------------------------

bool loadHeightmap(const std::string filename, double resolution, double blockheight, Mesh& mesh)
{
    double origin_x = 0;
    double origin_y = 0;

    png::image<png::gray_pixel> image(filename);

    Grid<unsigned char> heightmap(image.get_width(), image.get_height());
    Grid<int> vertex_index_map(image.get_width(), image.get_height(), -1);
    Grid<unsigned char> contour_map(image.get_width(), image.get_height(), false);

    for(int y = 0; y < image.get_height(); ++y)
    {
        for(int x = 0; x < image.get_width(); ++x)
        {
            heightmap(x, y) = image[y][x];
        }
    }

    for(int y = 0; y < heightmap.height(); ++y)
    {
        for(int x = 0; x < heightmap.width(); ++x)
        {
            unsigned char v = heightmap(x, y);

            if (v < 255)
            {
                std::vector<Vec2i> points, line_starts;
                findContours(heightmap, Vec2i(x, y), 0, points, line_starts, contour_map, true);

                unsigned int num_points = points.size();

                if (num_points > 2)
                {
                    double min_z = 0;
                    double max_z = (double)(255 - v) / 255 * blockheight;

                    std::list<TPPLPoly> testpolys;

                    TPPLPoly poly;
                    poly.Init(num_points);

                    for(unsigned int i = 0; i < num_points; ++i)
                    {
                        std::cout << points[i] << std::endl;

                        poly[i].x = points[i].x;
                        poly[i].y = points[i].y;

                        // Convert to world coordinates
                        double wx = points[i].x * resolution + origin_x;
                        double wy = (heightmap.height() - points[i].y - 1) * resolution + origin_y;

                        vertex_index_map(points[i].x, points[i].y) = mesh.addPoint(Vec3(wx, wy, min_z));
                        mesh.addPoint(Vec3(wx, wy, max_z));
                    }

                    testpolys.push_back(poly);

                    // Calculate side triangles
                    for(unsigned int i = 0; i < num_points; ++i)
                    {
                        int j = (i + 1) % num_points;
                        mesh.addTriangle(i * 2, i * 2 + 1, j * 2);
                        mesh.addTriangle(i * 2 + 1, j * 2 + 1, j * 2);
                    }

//                    for(unsigned int i = 0; i < line_starts.size(); ++i)
//                    {
//                        int x2 = line_starts[i].x;
//                        int y2 = line_starts[i].y;

//                        std::cout << "Bla: " << line_starts[i] << std::endl;

//                        while(heightmap(x2, y2) == v)
//                            ++x2;

//                        if (!contour_map(x2 - 1, y2) == 0)
//                        {
//                            // found a hole, so find the contours of this hole
//                            std::vector<Vec2i> hole_points;
//                            findContours(heightmap, Vec2i(x2 - 1, y2 + 1), 1, hole_points, line_starts, contour_map, false);

//                            if (hole_points.size() > 2)
//                            {
//                                TPPLPoly poly_hole;
//                                poly_hole.Init(hole_points.size());
//                                poly_hole.SetHole(true);

//                                for(unsigned int j = 0; j < hole_points.size(); ++j)
//                                {
//                                    poly_hole[j].x = hole_points[j].x;
//                                    poly_hole[j].y = hole_points[j].y;

//                                    // Convert to world coordinates
//                                    double wx = hole_points[j].x * resolution + origin_x;
//                                    double wy = (heightmap.height() - hole_points[j].y - 1) * resolution + origin_y;

//                                    vertex_index_map(hole_points[j].x, hole_points[j].y) = mesh.addPoint(Vec3(wx, wy, min_z));
//                                    mesh.addPoint(Vec3(wx, wy, max_z));
//                                }
//                                testpolys.push_back(poly_hole);

//                                // Calculate side triangles
//                                for(unsigned int j = 0; j < hole_points.size(); ++j)
//                                {
//                                    const Vec2i& hp1 = hole_points[j];
//                                    const Vec2i& hp2 = hole_points[(j + 1) % hole_points.size()];

//                                    int i1 = vertex_index_map(hp1.x, hp1.y);
//                                    int i2 = vertex_index_map(hp2.x, hp2.y);

//                                    mesh.addTriangle(i1, i1 + 1, i2);
//                                    mesh.addTriangle(i2, i1 + 1, i2 + 1);
//                                }
//                            }
//                        }
//                    }

                    TPPLPartition pp;
                    std::list<TPPLPoly> result;

                    if (!pp.Triangulate_EC(&testpolys, &result))
                    {
                        std::cout << "ed::models::getHeightMapShape() : ERROR: could not triangulate polygon." << std::endl;
                        return false;
                    }

                    for(std::list<TPPLPoly>::iterator it = result.begin(); it != result.end(); ++it)
                    {
                        TPPLPoly& cp = *it;

                        int i1 = vertex_index_map(cp[0].x, cp[0].y) + 1;
                        int i2 = vertex_index_map(cp[1].x, cp[1].y) + 1;
                        int i3 = vertex_index_map(cp[2].x, cp[2].y) + 1;

                        mesh.addTriangle(i1, i3, i2);
                    }

                    // Clear the represented part in the image
                    for(unsigned int i = 0; i < line_starts.size(); ++i)
                    {
                        int x2 = line_starts[i].x;
                        int y2 = line_starts[i].y;

                        while(heightmap(x2, y2) == v)
                        {

                            heightmap(x2, y2) = 255;
                            ++x2;
                        }
                    }

                }
            }
        }
    }

//    std::cout << shape->getMesh().getPoints().size() << " vertices" << std::endl;
//    std::cout << shape->getMesh().getTriangleIs().size() << " triangles" << std::endl;

    return true;
}

}
