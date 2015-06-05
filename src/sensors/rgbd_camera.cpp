#include "rgbd_camera.h"

#include "../world.h"
#include "../update.h"

//#include <opencv2/highgui/highgui.hpp>

// ----------------------------------------------------------------------------------------------------

RGBDCamera::RGBDCamera(Id id) : id_(id)
{
    rasterizer_.setCanvasSize(640, 480);
    rasterizer_.setFocalLengths(558, 558);
    rasterizer_.setOpticalCenter(320, 240);
    rasterizer_.setOpticalTranslation(0, 0);

    canvas_.set_capacity(rasterizer_.canvas_width() * rasterizer_.canvas_height());

    image_pkg_.add("depth", canvas_);
    image_pkg_.add("width", width_);
    image_pkg_.add("height", height_);

    mem_.create("rgbd", image_pkg_);

    canvas_.set_size(canvas_.capacity());
}

// ----------------------------------------------------------------------------------------------------

RGBDCamera::~RGBDCamera()
{
}

// ----------------------------------------------------------------------------------------------------

void RGBDCamera::Update(const World& world, WorldUpdate& u)
{
    // Get absolute camera pose
    Transform3 camera_pose = world.object(id_).abs_transform;

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

        rasterizer_.rasterize(o.mesh, camera_pose_inv * o.abs_transform, &canvas_[0]);
    }

    *width_ = rasterizer_.canvas_width();
    *height_ = rasterizer_.canvas_height();

//    std::cout << &(*width_) << std::endl;

//    char pkg_spec[1000];
//    image_pkg_.SerializeSpecification(pkg_spec);

//    {
//        io::Package pkg;
//        pkg.DeserializeSpecification(pkg_spec);

//        io::Vector<float> v_depth;
//        pkg.mapTo(image_pkg_.ptr());
//        pkg.map("depth", v_depth);

//        io::Value<uint32_t> w, h;
//        pkg.map("width", w);
//        pkg.map("height", h);

//        // Temporary: show using OpenCV
//        cv::Mat img(*h, *w, CV_32FC1, &v_depth[0]);
//        cv::imshow("depth", img / 8);
//        cv::waitKey(3);
//    }

}

