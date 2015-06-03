#ifndef RSIM_IO_VALUE_H_
#define RSIM_IO_VALUE_H_

#include <stdint.h>

namespace io
{

template<typename T>
class Value
{

    friend class Package;

public:

    Value() : ptr_(0) {}

    ~Value() {}

    T& operator*() { return *reinterpret_cast<T*>(*ptr_ + offset_); }

    T* operator->() { return reinterpret_cast<T*>(*ptr_ + offset_); }

private:

    unsigned char** ptr_;

    uint64_t offset_;

};

} // end namespace io

#endif
