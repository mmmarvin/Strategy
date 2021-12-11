#include "time_utility.h"

namespace gengine2d
{
namespace time_utils
{
  std::pair<int, int> convertSecToMinAndSec(double seconds)
  {
    int sec = seconds;
    int min = sec / 60;
    return std::make_pair(min % 60, sec % 60);
  }
}
}
