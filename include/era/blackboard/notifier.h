#ifndef ERA_BLACKBOARD_NOTIFIER_H_
#define ERA_BLACKBOARD_NOTIFIER_H_

namespace era
{
namespace blackboard
{

class Buffer;

// ----------------------------------------------------------------------------------------------------

template<typename T>
class Notifier
{
public:

    Notifier(Buffer* buffer = 0) : buffer_(0) {}

    virtual ~Notifier() {}

    virtual void callback(const T& value) {}

private:

    Buffer* buffer_;

};

}
}
#endif
