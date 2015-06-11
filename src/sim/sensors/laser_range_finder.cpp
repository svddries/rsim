#include "era/sim/sensors/laser_range_finder.h"

#include "era/sim/world.h"
#include "era/sim/update.h"

//#include <opencv2/highgui/highgui.hpp>

// ----------------------------------------------------------------------------------------------------

LaserRangeFinder::LaserRangeFinder(Id id) : id_(id)
{
    renderer_.setRangeLimits(0, 20);
    renderer_.setAngleLimits(-2, 2);
    renderer_.setNumBeams(1000);

    ranges_.set_capacity(renderer_.getNumBeams());

    scan_pkg_.add("ranges", ranges_);
    scan_pkg_.add("angle_min", angle_min_);
    scan_pkg_.add("angle_max", angle_max_);

    mem_.create("lrf", scan_pkg_);

    ranges_.set_size(renderer_.getNumBeams());
}

// ----------------------------------------------------------------------------------------------------

LaserRangeFinder::~LaserRangeFinder()
{
}

// ----------------------------------------------------------------------------------------------------

void LaserRangeFinder::Update(const World& world, WorldUpdate& u)
{
    // Get absolute lrf pose
    const Transform3& lrf_pose = world.object(id_).abs_transform;

    // Calculate inverse
    Transform3 lrf_pose_inverse = lrf_pose.inverse();

    // Clear ranges
    std::fill(ranges_.begin(), ranges_.end(), 0);

    for(const Object& o : world.objects())
    {
        if (o.mesh.triangles.empty())
            continue;

        renderer_.render(o.mesh, lrf_pose_inverse * o.abs_transform, &ranges_[0]);
    }

    *angle_min_ = renderer_.getAngleMin();
    *angle_max_ = renderer_.getAngleMax();
}
