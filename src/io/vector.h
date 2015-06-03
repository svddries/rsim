#ifndef RSIM_IO_VECTOR_H_
#define RSIM_IO_VECTOR_H_

#include <stdint.h>
#include <vector>

namespace io
{

template<typename T>
class Vector
{

    friend class Package;

public:

    Vector(uint64_t size = 0) : ptr_(0), size_(size)
    {

    }

    ~Vector() {}

//    void resize(uint64_t size)
//    {
//        if (ptr_)
//            throw;
//        size_ = size;
//    }

    T& operator[](uint64_t i) { return *reinterpret_cast<T*>(*ptr_ + offset_ + i * sizeof(T)); }

    uint64_t size() const { return size_; }

private:

    unsigned char** ptr_;

    uint64_t offset_;

    uint64_t size_;


};

} // end namespace io

#endif
