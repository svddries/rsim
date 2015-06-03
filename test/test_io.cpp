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

    {
        io::Value<float> width;
        io::Value<float> height;
        io::Value<TestStruct> test;
        io::Vector<int> v(10);

        pkg.add(width);
        pkg.add(height);
        pkg.add(test);
        pkg.add(v);

        std::cout << pkg.size() << std::endl;

        pkg.claim();

        *width = 1.23;
        *height = 4.56;

        test->a = 43;

        for(int i = 0; i < v.size(); ++i)
            v[i] = i;
    }

    {
        io::Package pkg2;

        io::Value<float> width;
        io::Value<float> height;
        io::Value<TestStruct> test;
        io::Vector<int> v(10);

        pkg2.add(width);
        pkg2.add(height);
        pkg2.add(test);
        pkg2.add(v);

        pkg2.claim(pkg.ptr());

        std::cout << *width << " " << *height << std::endl;

        std::cout << test->a << std::endl;

        for(int i = 0; i < v.size(); ++i)
            std::cout << v[i] << std::endl;
    }

//    std::cout << pkg.size() << std::endl;

    return 0;
}
