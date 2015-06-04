#include <iostream>

#include "../src/io/package.h"
#include "../src/io/value.h"
#include "../src/io/vector.h"

struct TestStruct
{
    int a;
    float f;
};

int main(int argc, char **argv)
{
    io::Package pkg;
    char specification[1000];

    {
        io::Value<float> width;
        io::Value<float> height;
        io::Value<TestStruct> test;
        io::Vector<int> v(10);

        pkg.add("width", width);
        pkg.add("height", height);
        pkg.add("test", test);
        pkg.add("v", v);

        std::cout << pkg.size() << std::endl;

        pkg.create();

        *width = 1.23;
        *height = 4.56;

        test->a = 43;

        v.resize(5);
        for(int i = 0; i < v.size(); ++i)
            v[i] = i;

        pkg.SerializeSpecification(specification);
    }

    {
        io::Package pkg2;
        pkg2.DeserializeSpecification(specification);

        io::Value<float> width;
        io::Value<float> height;
        io::Value<TestStruct> test;
        io::Vector<int> v;

        pkg.map("width", width);
        pkg.map("height", height);
        pkg.map("test", test);
        pkg.map("v", v);

        pkg2.mapTo(pkg.ptr());

        std::cout << *width << " " << *height << std::endl;

        std::cout << test->a << std::endl;

        for(int i = 0; i < v.size(); ++i)
            std::cout << v[i] << std::endl;
    }

//    std::cout << pkg.size() << std::endl;

    return 0;
}
