#ifndef DEBUG_COMMON_H
#define DEBUG_COMMON_H

#include "../entity/component/unit_components.h"

namespace strat
{
namespace string_converter_utils
{
  gengine2d::string_t to_string(component::UnitState::EUnitState state);
}
}

#endif // DEBUG_COMMON_H
