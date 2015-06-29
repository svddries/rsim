#ifndef RSIM_IO_TIME_BUFFER_H_
#define RSIM_IO_TIME_BUFFER_H_

#include <stdint.h>

namespace era
{
namespace io
{

struct TimeRef
{
    uint64_t time;  // time in nanoseconds
    uint64_t idx;   // buffer index
};

template<typename T>
class TimeBuffer
{

public:

    TimeBuffer(uint64_t capacity) : capacity_(capacity), i_latest_(0), i_current_(0)
    {
        time_refs_ = new ((unsigned char*)this + sizeof(TimeBuffer)) TimeRef[capacity_];
        buffer_ = new ((unsigned char*)time_refs_ + capacity_ * sizeof(TimeRef)) T[capacity_];
    }

    ~TimeBuffer()
    {
        for(unsigned int i = 0; i < capacity_; ++i)
            buffer_[i].~T();
    }

    T& Add(uint64_t time)
    {
        i_latest_ = i_current_;
        i_current_ = (i_current_ + 1) % capacity_;
        return buffer_[i_latest_];
    }



    T& latest() { return buffer_[i_latest_]; }

    static TimeBuffer* map(void* ptr)
    {
        return reinterpret_cast<TimeBuffer*>(ptr);
    }

    static TimeBuffer* create(uint64_t capacity)
    {
        char* data = new char[sizeof(TimeBuffer) + capacity * (sizeof(T) + sizeof(TimeRef))];
        return new (data) TimeBuffer(capacity);
    }

    uint64_t capacity() const { return capacity_; }

private:

    uint64_t capacity_;

    uint64_t i_latest_;

    uint64_t i_current_;

    TimeRef* time_refs_;

    T* buffer_;


};

} // end namespace io

} // end namespace era

#endif
