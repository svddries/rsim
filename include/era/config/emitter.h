#ifndef ERA_TUE_CONFIGURATION_EMITTER_H_
#define ERA_TUE_CONFIGURATION_EMITTER_H_

#include "era/config/data.h"

namespace era
{

namespace config
{

enum EmitType
{
    YAML,
    JSON
};

enum EmitOptions {
  EMIT_MINIMAL = 0x01,
  EMIT_QUOTED_KEYS = 0x02,
  EMIT_YAML = 0x04,
  EMIT_JSON = 0x08
};

void emit(const Data& data, std::ostream& out, int options = 0);

} // end namespace configuration

}

#endif
