#ifndef TUE_CONFIG_LOADERS_YAML_H_
#define TUE_CONFIG_LOADERS_YAML_H_

#include <string>
#include <istream>

namespace tue
{

namespace config
{

class ReaderWriter;

bool loadFromYAMLStream(std::istream& stream, ReaderWriter& config);

bool loadFromYAMLString(const std::string& string, ReaderWriter& config);

bool loadFromYAMLFile(const std::string& filename, ReaderWriter& config);

}

}

#endif
