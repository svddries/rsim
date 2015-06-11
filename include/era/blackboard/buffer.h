#ifndef ERA_BLACKBOARD_BUFFER_H_
#define ERA_BLACKBOARD_BUFFER_H_

#include <vector>
#include <assert.h>
#include <stdexcept>

#include "era/blackboard/notifier.h"

namespace era
{
namespace blackboard
{

// ----------------------------------------------------------------------------------------------------

namespace
{

template<typename T>
void remove(void* v)
{
    T* p = static_cast<T*>(v);
    p->~T();
}

}

// ----------------------------------------------------------------------------------------------------

class Buffer
{

public:

    Buffer() : i_next_(0), remover_(0) {}

    ~Buffer()
    {
        for(std::size_t i = 0; i < data_.size(); i += type_size_)
        {
            void* p = &data_[0] + i;
            remover_(p);
        }
    }

    template<typename T>
    void initialize(std::size_t size)
    {
        if (remover_)
            throw std::invalid_argument("In Buffer::initialize() (" + std::string(__FILE__) + "): Already initialized");

        type_size_ = sizeof(T);
        remover_ = remove<T>;
        type_index_ = get_type_index<T>();
        seq_ = 0;

        data_.resize(type_size_ * size);
    }

    template<typename T>
    void add(const T& v)
    {
        checkType<T>();
        new (&data_[0] + i_next_) T(v);
        i_next_ = (i_next_ + type_size_) % data_.size();
        ++seq_;

        for(void* p : notifiers_)
        {
            Notifier<T>* n = static_cast<Notifier<T>*>(p);
            n->callback(v);
        }
    }

    template<typename T>
    const T& latest() const
    {
        checkType<T>();
        std::size_t i = (i_next_ + data_.size() - type_size_) % data_.size();
        return *reinterpret_cast<const T*>(&data_[i]);
    }

    bool empty() const { return seq_ == 0; }

    std::size_t sequence_nr() const { return seq_; }

    std::size_t capacity() const { return data_.size() / type_size_; }

    template<typename T>
    void registerNotifier(Notifier<T>* n)
    {
        notifiers_.push_back(n);
    }

private:

    std::vector<char> data_;

    std::size_t i_next_;

    std::size_t seq_;

    void (*remover_)(void* v);

    std::size_t type_size_;

    int type_index_;

    std::vector<void*> notifiers_;

    static int next_type_index()
    {
        static int next_type_index_(0);
        return next_type_index_++;
    }

    template <typename T_>
    static int get_type_index()
    {
        static int result(next_type_index());
        return result;
    }

    template<typename T>
    void checkType() const
    {
        if (!remover_)
            throw std::invalid_argument("Buffer (" + std::string(__FILE__) + "): Not initialized");

        int type_index = get_type_index<T>();
        assert(type_index == type_index_);
        if (type_index != type_index_)
            throw std::invalid_argument("In Buffer::add() (" + std::string(__FILE__) + "): Incorrect type");
    }
};

}
}
#endif
