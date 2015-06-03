#ifndef RSIM_IO_PACKAGE_H_
#define RSIM_IO_PACKAGE_H_

#include <stdint.h>

#include "value.h"
#include "vector.h"

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
    void add(Value<T>& v)
    {
        v.offset_ = size_;
        v.ptr_ = &data_;
        size_ += sizeof(size_);
    }

    template<typename T>
    void add(Vector<T>& v)
    {
        v.offset_ = size_;
        v.ptr_ = &data_;
        size_ += sizeof(v.size()) + v.size() * sizeof(size_);
    }

    void claim()
    {
        if (data_is_mine_)
            delete data_;

        data_is_mine_ = true;
        data_ = new unsigned char[size_];
    }

    void claim(void* ptr)
    {
        if (data_is_mine_)
            delete data_;

        data_ = static_cast<unsigned char*>(ptr);
        data_is_mine_ = false;
    }

    uint64_t size() const { return size_; }

    void* ptr() { return data_; }

private:

    uint64_t size_;

    unsigned char* data_;

    bool data_is_mine_;

};

} // end namespace io

#endif
