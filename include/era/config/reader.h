#ifndef ERA_TUE_CONFIGURATION_READER_H_
#define ERA_TUE_CONFIGURATION_READER_H_

#include "era/config/data.h"

namespace era
{

namespace config
{

class Reader
{

public:

    Reader(Data& cfg, const Node& n = Node(0, MAP)) : data_(cfg), n_current_(n), i_next_array_item_(0)
    {
    }

    template<typename T>
    bool value(const std::string& key, T& value) const
    {
        std::map<std::string, Node>& map = data_.maps[n_current_.idx];
        std::map<std::string, Node>::const_iterator it = map.find(key);
        if (it == map.end())
            return false;

        const Variant& v = data_.values[it->second.idx];

        if (!v.getValue(value))
            return false;

        return true;
    }

    bool readGroup(const std::string& key)
    {
        std::map<std::string, Node>& map = data_.maps[n_current_.idx];
        std::map<std::string, Node>::const_iterator it = map.find(key);
        if (it == map.end())
            return false;

        n_current_ = it->second;
        return true;
    }

    void endGroup()
    {
        n_current_.idx = data_.map_parents[n_current_.idx];
        n_current_.type = MAP;
    }

    bool readArray(const std::string& key)
    {
        std::map<std::string, Node>& map = data_.maps[n_current_.idx];
        std::map<std::string, Node>::const_iterator it = map.find(key);
        if (it == map.end())
            return false;

        n_current_ = it->second;
        i_next_array_item_ = 0;

        return true;
    }

    void endArray()
    {
        if (n_current_.type != ARRAY && i_next_array_item_ > 0)
            n_current_.idx = data_.array_parents[data_.map_parents[n_current_.idx]];
        else
            n_current_.idx = data_.array_parents[n_current_.idx];

        n_current_.type = MAP;
    }

    bool nextArrayItem()
    {
        if (n_current_.type != ARRAY)
        {
            if (i_next_array_item_ == 0)
                return false;

            n_current_.idx = data_.map_parents[n_current_.idx];
            n_current_.type = ARRAY;
        }

        std::vector<Node>& array = data_.arrays[n_current_.idx];

        if (i_next_array_item_ >= array.size())
            return false;

        n_current_ = array[i_next_array_item_];
        ++i_next_array_item_;

        return true;
    }

private:

    Data& data_;
    Node n_current_;
    unsigned int i_next_array_item_;

};

}

}

#endif
