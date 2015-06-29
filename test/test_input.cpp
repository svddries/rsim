#include <vector>
#include <string>
#include <stdint.h>

// ----------------------------------------------------------------------------------------------------

struct LRFData
{
    std::vector<float> ranges;
};

struct LRFInfo
{
    double angle_min;
    double angle_max;
    double range_min;
    double range_max;
    uint32_t num_beams;
};

//              ------------
//  -- info --> |          |
//              |   MODEL  |
//  -- data --> |          |
//              ------------
//
// It does not really matter what the info and data are (maybe they're not even two different modalities)
// but that from it a model can be build which can be *queried* for information. Some questions:
//
// * How to hierarchically compose bigger models from smaller ones
//   * Assume we have an LRFModel and OdomModel, how to combine those? (maybe in a world model?)
// * Would it make sense to evaluate the models in a lazy manner, instead of using consumer-producer?
// * Is there one LRFModel, or are there many different models, depending on the task
// * Do we allow inheritance for models, or only composition?
// * Where to 'hide' the data API? If a field is added or changed, we should only have to change one computation
//   not all models
// * What kind of models do we have? Are there for example multiple LRF models, like a 'line model' which assumes
//   the world consists of lines; a 'grid model' which approximates free and occupied space using an occ. grid
//   * But these are world models!! Or maybe only sensor world models? (in sensor space)

// ----------------------------------------------------------------------------------------------------

struct RGBDData
{
    std::vector<uint8_t> rgb;
    std::vector<float> depth;
};

struct ImageInfo
{
    uint32_t width;
    uint32_t height;

};

struct RGBDInfo
{
    ImageInfo rgb;
    ImageInfo depth;
};

// ----------------------------------------------------------------------------------------------------

// What if not all joint data is produced by the same producer?
//    What if some joint states are received later than other?    (ROS uses timestamps)
//    What if some joint states are never received?               (ROS will fail: robot_state_publisher will fail)
// What if an odometer is defect and gives false information?     (ROS will fail: no uncertain representation, and no fallback)

struct JointData
{
    std::vector<float> joint_values;
};

struct JointInfo
{
    std::vector<std::string> joint_names;
};

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{



    return 0;
}
