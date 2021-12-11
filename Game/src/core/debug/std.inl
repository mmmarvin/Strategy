#include "std.h"

#include <iostream>

namespace gengine
{
  static std::ostream& std_out()
  {
    return std::cout;
  }

  static std::ostream& std_err()
  {
    return std::cerr;
  }
}
