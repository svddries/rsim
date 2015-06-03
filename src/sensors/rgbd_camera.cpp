#include "rgbd_camera.h"

#include "../world.h"
#include "../update.h"

#include <opencv2/highgui/highgui.hpp>

// ----------------------------------------------------------------------------------------------------

RGBDCamera::RGBDCamera(Id id) : id_(id)
{
    rasterizer_.setCanvasSize(640, 480);
    rasterizer_.setFocalLengths(558, 558);
    rasterizer_.setOpticalCenter(320, 240);
    rasterizer_.setOpticalTranslation(0, 0);

    canvas_.resize(rasterizer_.canvas_width() * rasterizer_.canvas_height());
}

// ----------------------------------------------------------------------------------------------------

RGBDCamera::~RGBDCamera()
{
}

// ----------------------------------------------------------------------------------------------------

void RGBDCamera::Update(const World& world, WorldUpdate& u)
{
    std::cout << "RGBDCamera::Update" << std::endl;

    // Get absolute camera pose
    Transform3 camera_pose = world.object(id_).abs_transform;

    std::cout << "Camera pose: " << camera_pose << std::endl;

    // Correct for rasterizer frame
    camera_pose.R = camera_pose.R * Mat3(1, 0, 0, 0, -1, 0, 0, 0, -1);

    // Calculate inverse
    Transform3 camera_pose_inv = camera_pose.inverse();

    // Clear the whole canvas
    std::fill(canvas_.begin(), canvas_.end(), 0);

    for(const Object& o : world.objects())
    {
        if (o.mesh.triangles.empty())
            continue;

        std::cout << "Relative pose: " << camera_pose_inv * o.abs_transform << std::endl;

        rasterizer_.rasterize(o.mesh, camera_pose_inv * o.abs_transform, &canvas_[0]);
    }

    // Temporary: show using OpenCV
    cv::Mat img(rasterizer_.canvas_height(), rasterizer_.canvas_width(), CV_32FC1, &canvas_[0]);
    cv::imshow("depth", img / 8);
    cv::waitKey(3);
}

