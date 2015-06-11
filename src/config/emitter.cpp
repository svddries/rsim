#include "era/config/emitter.h"

#include <iostream>
#include <sstream>

namespace era
{

namespace config
{

struct ParserImpl
{

    ParserImpl(const Data& data_) : data(data) {}

    // ----------------------------------------------------------------------------------------------------

    void emitVariant(const Variant& v, std::ostream& out)
    {
        if (v.isString())
        {
            out << "\"" << v << "\"";
        }
        else
        {
            out << v;
        }
    }

    // ----------------------------------------------------------------------------------------------------

    void emitYAML(const Data& data, const std::map<std::string, Node>& map, const std::string& indent, bool put_first_indent, std::ostream& out)
    {
        for(std::map<std::string, Node>::const_iterator it = map.begin(); it != map.end(); ++it)
        {
            if (put_first_indent)
                out << indent;

            if (quoted_keys)
                out << "\"" << it->first << "\"" << delimiter;
            else
                out << it->first << delimiter;

            const Node& n = it->second;
            if (n.type == VALUE)
            {
                emitVariant(data.values[n.idx], out);
                out << std::endl;
            }
            else if (n.type == MAP)
            {
                out << std::endl;
                emitYAML(data, data.maps[n.idx], indent + "    ", true, out);
            }
            else if (n.type == ARRAY)
            {
                out << std::endl;
                const std::vector<Node>& array = data.arrays[n.idx];
                for(std::vector<Node>::const_iterator it = array.begin(); it != array.end(); ++it)
                {
                    out << indent << "  - ";
                    emitYAML(data, data.maps[it->idx], indent + tab, false, out);
                }
            }

            put_first_indent = true;
        }
    }

    // ----------------------------------------------------------------------------------------------------

    void emitJSON(const Data& data, const std::map<std::string, Node>& map, const std::string& indent, std::ostream& out)
    {
        out << "{" << newline;

        std::string new_indent = indent + tab;

        for(std::map<std::string, Node>::const_iterator it = map.begin(); it != map.end(); ++it)
        {
            if (it != map.begin())
                out << "," << newline;

            out << new_indent;

            if (quoted_keys)
                out << "\"" << it->first << "\"";
            else
                out << it->first;

            out << delimiter;

            const Node& n = it->second;
            if (n.type == VALUE)
                emitVariant(data.values[n.idx], out);// << std::endl;
            else if (n.type == MAP)
            {
                emitJSON(data, data.maps[n.idx], new_indent, out);
            }
            else if (n.type == ARRAY)
            {
                out << "[" << std::endl;
                const std::vector<Node>& array = data.arrays[n.idx];
                for(std::vector<Node>::const_iterator it2 = array.begin(); it2 != array.end(); ++it2)
                {
                    if (it2 != array.begin())
                        out << "," << std::endl;

                    out << new_indent + tab;
                    emitJSON(data, data.maps[it2->idx], new_indent + tab, out);
                }
                out << newline << new_indent << "]";
            }
        }

        out << newline << indent << "}";
    }

    const Data& data;
    std::string newline;
    std::string tab;
    std::string delimiter;
    bool quoted_keys;

};

// ----------------------------------------------------------------------------------------------------

void emit(const Data& data, std::ostream& out, int options)
{
    if (data.maps.empty())
        return;

    ParserImpl p(data);
    p.delimiter = ":";

    if (!(options & EMIT_MINIMAL) || (options & EMIT_YAML))
    {
        p.newline = "\n";
        p.tab = "    ";
        p.delimiter += " ";
    }

    p.quoted_keys = (options & EMIT_JSON) || (options & EMIT_QUOTED_KEYS);

    std::string indent = "";

    if ((options & EMIT_MINIMAL) || (options & EMIT_JSON))
        p.emitJSON(data, data.maps[0], indent, out);
    else
        p.emitYAML(data, data.maps[0], indent, true, out);
}

} // end namespace configuration

}
