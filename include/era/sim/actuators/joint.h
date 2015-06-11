#ifndef RSIM_ACTUATORS_JOINT_H_
#define RSIM_ACTUATORS_JOINT_H_

#include "../behavior.h"
#include "../world.h"
#include "../update.h"

#include <limits>

class Joint : public Behavior
{

public:

    Joint(Id id) : id_(id), t_last_(0), pos_(0), reference_(0), vel_(0),
        pos_min_(std::numeric_limits<double>::quiet_NaN()),
        pos_max_(std::numeric_limits<double>::quiet_NaN()),
        max_vel_(0), max_acc_(0) {}

    void set_position(double pos) { pos_ = pos; }

    void set_position_limits(double min, double max)
    {
        pos_min_ = min;
        pos_max_ = max;
    }

    void set_max_velocity(double max_vel) { max_vel_ = max_vel; }

    void set_acceleration(double acc) { max_acc_ = acc; }

    void set_reference(double reference) { reference_ = reference; }

    void set_axis(const Vec3& v) { axis_ = v.normalized(); }

    void set_origin(const Transform3& t) { origin_ = t; }

    double position() const { return pos_; }

protected:

    bool UpdatePos(const World& world)
    {
        double dt = world.time() - t_last_;
        t_last_ = world.time();

        if (pos_ == reference_ && vel_ == 0)
            return false;

        if (reference_ > pos_)
            vel_ = std::min(max_vel_, vel_ + dt * max_acc_);
        else
            vel_ = std::max(-max_vel_, vel_ - dt * max_acc_);

        if (std::abs(vel_) < 1e-10)
        {
            vel_ = 0;
            return false;
        }

        pos_ += dt * vel_;
        if (pos_ < pos_min_)
        {
            pos_ = pos_min_;
            vel_ = 0;
        }
        else if (pos_ > pos_max_)
        {
            pos_ = pos_max_;
            vel_ = 0;
        }

        return true;
    }

    Id id_;

    Time t_last_;

    double pos_;

    double reference_;

    double vel_;

    double pos_min_, pos_max_;

    double max_vel_, max_acc_;

    Vec3 axis_;

    Transform3 origin_;

};


#endif
