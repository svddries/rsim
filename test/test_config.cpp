#include "era/config/data.h"
#include "era/config/yaml.h"

#include "era/config/emitter.h"

#include <iostream>

int main(int argc, char **argv)
{
    era::config::Data data;

    if (argc > 1)
    {
        std::cout << "Parsing file: " << argv[1] << std::endl;
        era::config::yaml::parseFile(argv[1], data);
    }
    else
    {
        std::string yaml = "{a: 123, b: 456, c: { e: [{f: 456}, {f: 789}]}}";
        era::config::yaml::parseString(yaml, data);
    }

    era::config::emit(data, std::cout, era::config::EMIT_YAML);
    std::cout << "---------------------" << std::endl;
    era::config::emit(data, std::cout, era::config::EMIT_JSON);

    return 0;
}
