#ifndef ERA_BLACKBOARD_OBSERVER_H_
#define ERA_BLACKBOARD_OBSERVER_H_

#include "era/blackboard/buffer.h"

namespace era
{
namespace blackboard
{

// ----------------------------------------------------------------------------------------------------

template<typename T>
class Observer
{

public:

    Observer(Buffer* buffer = 0) : buffer_(buffer), last_seq_nr_(-1) {}

    ~Observer()
    {
    }

    void initialize(Buffer* buffer)
    {
        buffer_ = buffer;
    }

    bool changed()
    {
        bool changed = buffer_->sequence_nr() != last_seq_nr_;
        last_seq_nr_ = buffer_->sequence_nr();
        return changed;
    }

    const T& latest() const
    {
        return buffer_->latest<T>();
    }

private:

    Buffer* buffer_;

    std::size_t last_seq_nr_;
};

}
}
#endif
