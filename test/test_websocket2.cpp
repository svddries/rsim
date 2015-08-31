#include <iostream>
#include <era/visualization/websocket_scene_graph.h>

#include <unistd.h>

int main()
{
    era::WebsocketSceneGraph g;
    g.InitComm(9002);

    g.addNode("robot", "world", era::Transform3(era::Mat3::identity(), era::Vec3(1, 2, 3)));
    g.addNode("laser", "robot", era::Transform3(era::Mat3::identity(), era::Vec3(0, 0, 0.35)));
    g.setData("laser", "angle_min", "-2.2");
    g.setPose("robot", era::Transform3(era::Mat3::identity(), era::Vec3(1, 2, 4)));

    std::cout << g.CreateDifferenceMessage(3) << std::endl;


    while(true)
    {
        g.PollRequests();
        usleep(10000);
    }

//    g.setPose("robot", ...);
//    g.setData("laser, "\"ranges\":[1,2,3,4,5]");

    return 0;
}
