#include "config_utility.h"

#include "../utility/stl_utility.h"

namespace gengine2d
{
namespace config_utils
{
  string_t versionToString(unsigned int major, unsigned int minor, unsigned int patch)
  {
    return stl_utils::to_string(major) + "." + stl_utils::to_string(minor) + stl_utils::to_string(patch);
  }
}
}
