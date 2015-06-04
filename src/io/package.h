#ifndef RSIM_IO_PACKAGE_H_
#define RSIM_IO_PACKAGE_H_

#include <stdint.h>
#include <map>
#include <string>

#include "value.h"
#include "vector.h"

namespace
{

struct SimpleByteWriter
{
    SimpleByteWriter(char* data_) : data(data_), i(0) {}

    template<typename T>
    void write(const T& d)
    {
        *reinterpret_cast<T*>(&data[i]) = d;
        i += sizeof(T);
    }

    void writeString(const char* s)
    {
        int j = 0;
        do { data[i++] = s[j]; } while (s[j++] != '\0');
    }

    char* data;
    uint64_t i;

};

struct SimpleByteReader
{
    SimpleByteReader(const char* data_) : data(data_), i(0) {}

    template<typename T>
    const T& read()
    {
        uint64_t i_old = i;
        i += sizeof(T);
        return *reinterpret_cast<const T*>(&data[i_old]);
    }

    const char* readString()
    {
        const char* s = &data[i];
        while(data[i] != '\0') ++i;
        ++i;
        return s;
    }

    const char* data;
    uint64_t i;

};

}

namespace io
{

class Package
{

public:

    Package() : size_(0), data_(0), data_is_mine_(false) {}

    ~Package()
    {
        if (data_is_mine_)
            delete data_;
    }

    template<typename T>
    void add(const char* name, Value<T>& v)
    {
        v.offset_ = size_;
        v.ptr_ = &data_;
        value_specs_[name] = size_;
        size_ += sizeof(size_);
    }

    template<typename T>
    void map(const char* name, Value<T>& v)
    {
        auto it = value_specs_.find(name);
        if (it == value_specs_.end())
            throw;

        v.offset_ = it->second;
        v.ptr_ = &data_;
    }

    template<typename T>
    void add(const char* name, Vector<T>& v)
    {
        v.offset_ = size_;
        v.ptr_ = &data_;
        size_ += sizeof(v.size()) + v.size() * sizeof(size_);
    }

    void create()
    {
        if (data_is_mine_)
            delete data_;

        data_is_mine_ = true;
        data_ = new unsigned char[size_];
    }

    void mapTo(void* ptr)
    {
        if (data_is_mine_)
            delete data_;

        data_ = static_cast<unsigned char*>(ptr);
        data_is_mine_ = false;
    }

    uint64_t size() const { return size_; }

    void* ptr() { return data_; }

    void SerializeSpecification(char* specification)
    {
        SimpleByteWriter w(specification);
        w.write<uint32_t>(0); // Write the version
        w.write((uint64_t)value_specs_.size()); // Write the size

        for(const auto& s : value_specs_)
        {
            w.writeString(s.first.c_str());
            w.write(s.second);
        }
    }

    void DeserializeSpecification(const char* specification)
    {
        SimpleByteReader r(specification);
        uint32_t version = r.read<uint32_t>();
        uint64_t size = r.read<uint64_t>();

        for(uint64_t i = 0; i < size; ++i)
        {
            const char* name = r.readString();
            uint64_t offset = r.read<uint64_t>();
            value_specs_[name] = offset;
            std::cout << name << ": " << offset << std::endl;
        }
    }

private:

    uint64_t size_;

    unsigned char* data_;

    bool data_is_mine_;

    std::map<std::string, uint64_t> value_specs_;

};

} // end namespace io

#endif
