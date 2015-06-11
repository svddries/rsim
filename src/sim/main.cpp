#include "era/sim/world.h"
#include "era/sim/behavior.h"
#include "era/sim/update.h"

#include "era/sim/actuators/prismatic_joint.h"
#include "era/sim/actuators/rotational_joint.h"
#include "era/sim/sensors/rgbd_camera.h"
#include "era/sim/sensors/laser_range_finder.h"

#include "rapidxml/rapidxml_utils.hpp"
#include <sstream>
#include <map>

#include "era/geometry/primitives.h"

#include <unistd.h> // usleep

#include "era/geometry/heightmap.h"

#include "era/filesystem.h"
#include "era/config/data.h"
#include "era/config/reader.h"
#include "era/config/yaml.h"

using namespace era;
using namespace era::sim;

// ----------------------------------------------------------------------------------------------------

bool splitFloats(const std::string &s, char delim, double* elems, int n_elems) {
    std::stringstream ss(s);
    std::string item;

    int i = 0;
    while (std::getline(ss, item, delim))
    {
        if (i > n_elems)
            return false;

        elems[i++] = atof(item.c_str());
    }

    return (n_elems + 1 == i);
}

// ----------------------------------------------------------------------------------------------------

bool parseTransform(rapidxml::xml_node<>* n, Transform3& t)
{
    if (!n)
        return false;

    rapidxml::xml_attribute<>* a_xyz = n->first_attribute("xyz");
    rapidxml::xml_attribute<>* a_rpy = n->first_attribute("rpy");

    if (a_xyz)
    {
        double elems[3];
        splitFloats(a_xyz->value(), ' ', elems, 3);
        t.t = Vec3(elems[0], elems[1], elems[2]);
    }
    else
    {
        t.t = Vec3(0, 0, 0);
    }

    if (a_rpy)
    {
        double elems[3];
        splitFloats(a_rpy->value(), ' ', elems, 3);
        t.R.setRPY(elems[0], elems[1], elems[2]);
    }
    else
    {
        t.R = Mat3::identity();
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool loadURDF(const std::string& filename, World& world, Id& root_id)
{
    rapidxml::file<> xmlFile(filename.c_str()); // Default template is char
    rapidxml::xml_document<> doc;
    doc.parse<0>(xmlFile.data());

    rapidxml::xml_node<>* n_robot = doc.first_node("robot");
    if (!n_robot)
        return false;

//    // Parse links
//    for(rapidxml::xml_node<>* n_link = n_robot->first_node("link"); n_link; n_link = n_link->next_sibling("link"))
//    {
//        rapidxml::xml_attribute<>* a_name = n_link->first_attribute("name");
//        if (!a_name)
//        {
//            std::cout << "Link without a name!" << std::endl;
//            continue;
//        }

//        world.AddObject(Object(), )

//        if (a_name)
//            std::cout << a_name->value() << std::endl;

////        std::cout << n_link->value() << std::endl;
//    }

    std::map<std::string, Id> link_map;

    // Parse joints
    for(rapidxml::xml_node<>* n_joint = n_robot->first_node("joint"); n_joint; n_joint = n_joint->next_sibling("joint"))
    {
        rapidxml::xml_attribute<>* a_name = n_joint->first_attribute("name");
        rapidxml::xml_attribute<>* a_type = n_joint->first_attribute("type");

        if (!a_name || !a_type)
        {
            std::cout << "Joint has no name or type" << std::endl;
            continue;
        }

        rapidxml::xml_node<>* n_parent = n_joint->first_node("parent");
        rapidxml::xml_node<>* n_child = n_joint->first_node("child");

        if (!n_parent || !n_child)
        {
            std::cout << "Joint has no parent or child" << std::endl;
            continue;
        }

        rapidxml::xml_attribute<>* a_parent = n_parent->first_attribute("link");
        rapidxml::xml_attribute<>* a_child = n_child->first_attribute("link");

        const char* parent_name = a_parent->value();
        const char* child_name = a_child->value();

        Id parent_id, child_id;

        // Look-up parent
        auto it_parent = link_map.find(parent_name);
        if (it_parent == link_map.end())
        {
            Object obj;
            obj.name = parent_name;
            parent_id = world.AddObject(obj);
            link_map[parent_name] = parent_id;
        }
        else
            parent_id = it_parent->second;

        // Look-up child
        auto it_child = link_map.find(child_name);
        if (it_child == link_map.end())
        {
            Object obj;
            obj.name = child_name;
            child_id = world.AddObject(obj);
            link_map[child_name] = child_id;
        }
        else
            child_id = it_child->second;

        // Create joint       
        Transform3 origin;
        if (!parseTransform(n_joint->first_node("origin"), origin))
            origin = Transform3::identity();

        world.SetObjectParent(child_id, parent_id, origin);

        std::string joint_type(a_type->value());

        Joint* joint = 0;
        if (joint_type == "prismatic")
        {
            joint = new PrismaticJoint(child_id);
        }
        else if (joint_type == "revolute")
        {
            joint = new RotationalJoint(child_id);
        }
        else if (joint_type == "fixed")
        {
            // Already set the origin, so do nothing
        }
        else
        {
            std::cout << "Unknown joint type: " << joint_type << std::endl;
        }

        if (std::string(child_name) == "top_kinect/openni_depth_optical_frame")
            world.AddBehavior(new RGBDCamera(child_id));

        if (std::string(child_name) == "base_laser")
            world.AddBehavior(new LaserRangeFinder(child_id));

        std::cout << child_name << std::endl;

        if (joint)
        {
            joint->set_origin(origin);

            Transform3 axis;
            if (parseTransform(n_joint->first_node("axis"), axis))
            {
                joint->set_axis(axis.t);
            }
            else
                std::cout << "No axis specified in joint " << a_name->value() << std::endl;

            joint->set_position_limits(-1000, 1000);
            joint->set_max_velocity(0.5);
            joint->set_acceleration(10);            

//            if (std::string(a_name->value()) == "neck_tilt_joint")
//                joint->set_reference(-0.3);

//            if (std::string(a_name->value()) == "neck_pan_joint")
//                joint->set_reference(-0.3);

            world.AddBehavior(joint);
        }
    }

    // Find the root
    for(auto it : link_map)
    {
        Object& obj = world.object(it.second);
        if (!obj.has_parent())
        {
            root_id = it.second;
            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------------------------------

class TestBehavior : public Behavior
{

public:

    TestBehavior(Id id) : id_(id) {}

    void Update(const World& world, WorldUpdate& u)
    {
        Transform3 t;
        t.R.setRPY(0, 0, 2 * world.time());
        t.t = Vec3(4, sin(2 * world.time()), 0.0);
        u.setTransform(id_, t);
    }

private:

    Id id_;

};

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        std::cout << "Please specify simulation config" << std::endl;
        return 1;
    }

    std::string config_filename = argv[1];

    era::config::Data data;
    if (!era::config::yaml::parseFile(config_filename, data))
        return 1;

    era::config::Reader r(data);

    std::string urdf_filename, heightmap_filename;
    r.value("urdf", urdf_filename);
    r.value("world", heightmap_filename);

    // Make the paths absolute
    std::string config_dir = era::filesystem::getParentPath(config_filename);
    urdf_filename = config_dir + urdf_filename;
    heightmap_filename = config_dir + heightmap_filename;

    World w;

    Id robot_id;
    if (!loadURDF(urdf_filename, w, robot_id))
    {
        std::cout << "Could not load URDF" << std::endl;
        return 1;
    }

    // Add robot to the world
    Mat3 m;
    m.setRPY(0, 0, 0);
    w.SetObjectParent(robot_id, w.root(), Transform3(m, Vec3(0, 0, 0)));

    // Add a box to the world
    Object box;
    box.name = "box";
//    createBox(Vec3(-0.4, -0.4, -0.4), Vec3(0.4, 0.4, 0.4), box.mesh);
    era::loadHeightmap(heightmap_filename, 0.01, 0.5, box.mesh);

    Id box_id = w.AddObject(box);
    w.SetObjectParent(box_id, w.root(), Transform3(Mat3::identity(), Vec3(4, 0.5, 0.3)));
    w.AddBehavior(new TestBehavior(box_id));

    while(true)
    {
        w.Update(0.01);
        usleep(10000);
    }

    return 0;
}
