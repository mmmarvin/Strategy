#ifndef STD_H
#define STD_H

#include <iosfwd>

namespace gengine
{
  static std::ostream& std_out();
  static std::ostream& std_err();
}

#include "std.inl"

#endif // STD_H
