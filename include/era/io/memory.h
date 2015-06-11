#ifndef RSIM_IO_MEMORY_H_
#define RSIM_IO_MEMORY_H_

#include <vector>
#include <stdint.h>
#include <string.h> // memcpy

namespace io
{

class Buffer
{

public:

    Buffer() {}

    virtual ~Buffer() {}

    virtual uint64_t size() const = 0;

    virtual void* allocate(uint64_t size) = 0;

};

class GrowingBuffer : public Buffer
{

public:

    GrowingBuffer() {}
    ~GrowingBuffer() {}

    uint64_t size() const { return data_.size(); }

    void* allocate(uint64_t size)
    {
        data_.resize(data_.size() + size);
        return &data_.back() - size + 1;
    }

    const char* ptr() const
    {
        return &data_[0];
    }

    void copyTo(void* p)
    {
        memcpy(p, ptr(), data_.size());
    }

private:

    std::vector<char> data_;

};

} // end namespace io

#endif
