#ifndef RSIM_SENSORS_LRF_RENDERER_H_
#define RSIM_SENSORS_LRF_RENDERER_H_

#include "era/geometry/math.h"

#include <vector>

namespace era
{

class Mesh;

namespace sim
{

class LRFRenderer
{

public:

    LRFRenderer();

    ~LRFRenderer();

    void render(const Mesh& mesh, const Transform3f& pose, float* ranges) const;

    void renderLine(const Vec2& p1, const Vec2& p2, float* ranges) const;

    void setAngleLimits(double min, double max);

    void setRangeLimits(double min, double max);

    void setNumBeams(int n);

    double getAngleMin() const;

    double getAngleMax() const;

    double getAngleIncrement() const;

    double getRangeMin() const;

    double getRangeMax() const;

    uint64_t getNumBeams() const;

    Vec2 rangeToPoint(float range, int i) const;

    const Vec2& getRayDirection(int i) const;

    void rangesToPoints(const float* ranges, Vec2* points) const;

    int getAngleUpperIndex(double angle) const;

    int getAngleUpperIndex(double x, double y) const;

    static Vec2 polarTo2D(double angle, double range);

    static double getAngle(double x, double y);

    inline const std::vector<Vec2>& rayDirections() const { return ray_dirs_; }

protected:

    double a_min_, a_max_;

    double range_min_, range_max_;

    int num_beams_;

    std::vector<Vec2> ray_dirs_;

    double slope_factor_;
    std::vector<int> slope_to_index_[8];
    int i_half_circle_;

    void calculateRays();

};

}
}

#endif
