#ifndef RSIM_UPDATE_H_
#define RSIM_UPDATE_H_

#include "types.h"
#include "math.h"

#include <vector>

struct WorldUpdate
{

    void setTransform(Id id, const Transform3& t) { transforms.push_back(std::make_pair(id, t)); }

    std::vector<std::pair<Id, Transform3> > transforms;

};

#endif
