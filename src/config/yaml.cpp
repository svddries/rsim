#include "era/config/yaml.h"
#include "era/config/reader.h"
#include "era/config/writer.h"
#include "era/config/data.h"

// YAML parsing
#include <fstream>

#include "yaml-cpp/yaml.h"

#include <iostream>
#include <stdlib.h>

namespace era
{

namespace config
{

namespace yaml
{

// ----------------------------------------------------------------------------------------------------

Variant yamlScalarToVariant(const YAML::Node& n, std::string& error)
{
    try
    {
        return Variant(n.as<int>());
    }
    catch (const YAML::BadConversion& e) {}

    try
    {
        return Variant(n.as<double>());
    }
    catch (const YAML::BadConversion& e) {}

    return Variant(n.as<std::string>());
}

// ----------------------------------------------------------------------------------------------------

bool loadFromYAMLNode(const YAML::Node& node, Writer& config)
{

    for(YAML::const_iterator it = node.begin(); it != node.end(); ++it)
    {
        std::string key = it->first.as<std::string>();
        const YAML::Node& n = it->second;

        switch (n.Type())
        {
        case YAML::NodeType::Scalar:
        {
            std::string error;
            Variant v = yamlScalarToVariant(n, error);
            if (v.valid())
                config.setValue(key, v);
            else
                config.addError("While reading key '" + key +"': " + error);
            break;
        }
        case YAML::NodeType::Sequence:
        {
            config.writeArray(key);

            for(YAML::const_iterator it2 = n.begin(); it2 != n.end(); ++it2)
            {
                const YAML::Node& n2 = *it2;
                if (n2.Type() != YAML::NodeType::Map)
                {
                    config.addError("Sequences must only contains maps");
                    return false;
                }
                else
                {
                    config.addArrayItem();
                    loadFromYAMLNode(n2, config);
                    config.endArrayItem();
                }
            }

            config.endArray();

            break;
        }
        case YAML::NodeType::Map:
            config.writeGroup(key);
            loadFromYAMLNode(n, config);
            config.endGroup();
            break;
        case YAML::NodeType::Null:
            break;
        }
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool parse(const YAML::Node& doc, Data& config)
{
    if (doc.Type() != YAML::NodeType::Map)
    {
        std::cout << "Root of the config file must be a map." << std::endl;
        return false;
    }

    Writer w(config);

    return loadFromYAMLNode(doc, w);
}

// ----------------------------------------------------------------------------------------------------

bool parseStream(std::istream& stream, Data& config)
{
    try
    {
        YAML::Node doc = YAML::Load(stream);
        parse(doc, config);
    }
    catch(YAML::Exception& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool parseString(const std::string& string, Data& config)
{
    try
    {
        YAML::Node doc = YAML::Load(string);
        parse(doc, config);
    }
    catch(YAML::Exception& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool parseFile(const std::string& filename, Data& config)
{
    try
    {
        YAML::Node doc = YAML::LoadFile(filename);
        parse(doc, config);
    }
    catch(YAML::Exception& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }

    return true;
}

}

}

}
