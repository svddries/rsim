#ifndef ERA_CONFIG_YAML_H_
#define ERA_CONFIG_YAML_H_

#include <string>
#include <istream>

namespace era
{

namespace config
{

struct Data;

namespace yaml
{

bool parseStream(std::istream& stream, Data& config);

bool parseString(const std::string& string, Data& config);

bool parseFile(const std::string& filename, Data& config);

}

}

}

#endif
