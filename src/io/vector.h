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

    Vector(uint64_t capacity = 0) : ptr_(0), capacity_(capacity)
    {

    }

    ~Vector() {}

    void resize(uint64_t s)
    {
        *reinterpret_cast<uint64_t*>(*ptr_ + offset_) = s;
    }

    T& operator[](uint64_t i) { return *reinterpret_cast<T*>(*ptr_ + offset_ + sizeof(uint64_t) + i * sizeof(T)); }

    uint64_t size() const { return *reinterpret_cast<uint64_t*>(*ptr_ + offset_); }

    uint64_t byte_size() const { return 2 * sizeof(uint64_t) + capacity_ * sizeof(T); }

private:

    unsigned char** ptr_;

    uint64_t offset_;

    uint64_t capacity_;

};

} // end namespace io

#endif
