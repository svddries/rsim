#include <iostream>

#include "../src/io/package.h"
#include "../src/io/value.h"
#include "../src/io/vector.h"
#include "../src/io/shared_memory.h"

#include "../src/sensors/lrf_renderer.h"

#include <opencv2/highgui/highgui.hpp>

int main(int argc, char **argv)
{
    io::SharedMemory mem;

    io::Package pkg;
    mem.open("lrf", pkg);

    io::Value<float> angle_min;
    io::Value<float> angle_max;
    io::Vector<float> v_ranges;

    pkg.map("ranges", v_ranges);
    pkg.map("angle_min", angle_min);
    pkg.map("angle_max", angle_max);

    while(true)
    {
        if (*angle_min != 0 || *angle_max != 0)
        {
            LRFRenderer lrf;
            lrf.setAngleLimits(*angle_min, *angle_max);
            lrf.setNumBeams(v_ranges.size());
            lrf.setRangeLimits(0, 20);

            std::vector<Vec2> points(lrf.getNumBeams());
            lrf.rangesToPoints(&v_ranges[0], &points[0]);

            cv::Mat img(500, 500, CV_32FC1, cv::Scalar(0));

            for(const Vec2& p : points)
            {
                Vec2 p_img = -p * 50 + Vec2(img.cols / 2, img.rows / 2);
                cv::circle(img, cv::Point(p_img.y, p_img.x), 1, cv::Scalar(1), 1);
            }


            cv::imshow("ranges", img);
        }

        cv::waitKey(3);
    }

    return 0;
}
