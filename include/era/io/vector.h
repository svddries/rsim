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

    // ---------------------------------------

    template<typename K>
    class _Iterator : public std::iterator<std::forward_iterator_tag, K>
    {

    public:

        _Iterator(K* p) : p_(p) {}

        _Iterator& operator++() { ++p_; return *this; }
        _Iterator operator++(int) { return _Iterator(p_++); }

        K* operator->() const { return p_; }

        bool operator==(const _Iterator& rhs) { return p_ == rhs.p_; }

        bool operator!=(const _Iterator& rhs) { return p_ != rhs.p_; }

        K& operator*() { return *p_; }

    private:

        K* p_;
    };

    typedef _Iterator<T> iterator;
    typedef _Iterator<const T> const_iterator;

    // ---------------------------------------

    Vector(uint64_t capacity = 0) : ptr_(0), capacity_(capacity)
    {

    }

    ~Vector() {}

    void set_capacity(uint64_t s)
    {
        capacity_ = s;
    }

    void set_size(uint64_t s)
    {
        *reinterpret_cast<uint64_t*>(*ptr_ + offset_) = s;
    }

    T& operator[](uint64_t i) { return *reinterpret_cast<T*>(*ptr_ + offset_ + sizeof(uint64_t) + i * sizeof(T)); }

    uint64_t size() const { return *reinterpret_cast<uint64_t*>(*ptr_ + offset_); }

    uint64_t byte_size() const { return 2 * sizeof(uint64_t) + capacity_ * sizeof(T); }

    uint64_t capacity() const { return capacity_; }

    iterator begin() { return iterator(&operator[](0)); }
    iterator end() { return iterator(&operator[](size())); }

    const_iterator begin() const { return const_iterator(&operator[](0)); }
    const_iterator end() const { return const_iterator(&operator[](size())); }

private:

    unsigned char** ptr_;

    uint64_t offset_;

    uint64_t capacity_;

};

} // end namespace io

#endif
