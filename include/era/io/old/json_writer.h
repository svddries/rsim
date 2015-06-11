#ifndef ERA_TUE_IO_JSON_WRITER_H_
#define ERA_TUE_IO_JSON_WRITER_H_

#include "era/io/old/writer.h"

#include <sstream>

namespace era
{

namespace io
{

class JSONWriter : Writer
{

public:

    JSONWriter(Data& data) : Writer(data), add_comma_(false)
    {
        addString("{");
    }

    ~JSONWriter() {}

    void writeGroup(const std::string& name)
    {
        if (add_comma_)
            addString(",");

        addString("\"" + name + "\":{");
        type_stack_.push_back('g');
        add_comma_ = false;
    }

    void endGroup()
    {
        addString("}");
        type_stack_.pop_back();
        add_comma_ = true;
    }

    void writeValue(const std::string& key, float f)
    {
        if (add_comma_)
            addString(",");

        std::stringstream s;
        s << "\"" << key << "\":" << f;
        addString(s.str());

        add_comma_ = true;
    }

    void writeValue(const std::string& key, int i)
    {
        if (add_comma_)
            addString(",");

        std::stringstream s;
        s << "\"" << key << "\":" << i;
        addString(s.str());

        add_comma_ = true;
    }

    void writeValue(const std::string& key, const std::string& s)
    {
        if (add_comma_)
            addString(",");

        addString("\"" + key + "\":\"" + s + "\"");

        add_comma_ = true;
    }

    void writeValue(const std::string& key, const float* fs, std::size_t size)
    {
        if (add_comma_)
            addString(",");
        addString("\"" + key + "\":[");

        if (size > 0)
        {
            std::stringstream s;
            s << fs[0];
            addString(s.str());

            for(unsigned int i = 1; i < size; ++i)
            {
                std::stringstream s;
                s << fs[i];
                addString("," + s.str());
            }
        }
        addString("]");
    }

    void writeValue(const std::string& key, const int* is, std::size_t size)
    {
        if (add_comma_)
            addString(",");
        addString("\"" + key + "\":[");

        if (size > 0)
        {
            std::stringstream s;
            s << is[0];
            addString(s.str());

            for(unsigned int i = 1; i < size; ++i)
            {
                std::stringstream s;
                s << is[i];
                addString("," + s.str());
            }
        }
        addString("]");
    }

    void writeValue(const std::string& key, const std::string* ss, std::size_t size)
    {
        if (add_comma_)
            addString(",");
        addString("\"" + key + "\":[");

        if (size > 0)
        {
            addString("\"" + ss[0] + "\"");
            for(unsigned int i = 1; i < size; ++i)
                addString(",\"" + ss[i] + "\"");
        }
        addString("]");
    }

    void writeArray(const std::string& key)
    {
        if (add_comma_)
            addString(",");

        addString("[");
        type_stack_.push_back('a');
        add_comma_ = false;
    }

    void addArrayItem() {}
    void endArrayItem() {}

    void endArray()
    {
        addString("]");
        type_stack_.pop_back();
        add_comma_ = true;
    }

    void finish()
    {
        while(!type_stack_.empty())
        {
            char t = type_stack_.back();
            if (t == 'g')
                endGroup();
            else if (t == 'a')
                endArray();
        }
        addString("}");
    }

private:

    inline void addString(const std::string& str)
    {
        data_.insert(data_.end(), str.begin(), str.end());
    }

    bool add_comma_;
    std::vector<char> type_stack_;

};

}

} // end namespace era

#endif
