#include "world.h"
#include "behavior.h"
#include "update.h"

class TestBehavior : public Behavior
{

public:

    TestBehavior(Id id) : id_(id) {}

    void update(const World& world, Update& u)
    {
        Time angle = world.time();

        Transform3 t;
        t.t = Vec3(0, 0, 0);
        t.R.setRPY(0, 0, angle);

        u.setTransform(id_, t);
    }

private:

    Id id_;

};

int main(int argc, char **argv)
{
    World w;
    Id id = w.AddObject(Object(), Transform3::identity());

    w.AddBehavior(new TestBehavior(id));

    for(unsigned int i = 0; i < 100; ++i)
    {
        w.update(0.01);
        std::cout << w.object(0).transform << std::endl;
    }

    return 0;
}
