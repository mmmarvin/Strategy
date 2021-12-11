#include "float_utility.h"

#include <cmath>
#include <limits>
#include "general_utility.h"

namespace gengine2d
{
namespace float_utils
{
  bool float_equal(float a, float b)
  {
    return general_utils::fast_abs(b - a) <= std::numeric_limits<float>::epsilon();
  }
}
}
