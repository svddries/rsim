#include <iostream>

#include "../src/io/package.h"
#include "../src/io/value.h"
#include "../src/io/vector.h"
#include "../src/io/shared_memory.h"

#include <opencv2/highgui/highgui.hpp>

int main(int argc, char **argv)
{
    io::SharedMemory mem;

    io::Package pkg;
    mem.open("rgbd", pkg);

    io::Value<uint32_t> width;
    io::Value<uint32_t> height;
    io::Vector<float> v_depth;

    pkg.map("depth", v_depth);
    pkg.map("width", width);
    pkg.map("height", height);

    while(true)
    {
        if (*width > 0 && *height > 0)
        {
            cv::Mat img(*height, *width, CV_32FC1, &v_depth[0]);
            cv::imshow("depth", img / 8);
        }

        cv::waitKey(3);
    }

    return 0;
}
