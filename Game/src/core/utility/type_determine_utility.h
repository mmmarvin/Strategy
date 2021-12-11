#ifndef TYPE_DETERMINE_UTILITY_H
#define TYPE_DETERMINE_UTILITY_H

#include "../type.h"

namespace gengine2d
{
namespace type_determine_utils
{
  bool isBoolean(const string_t& str);
  bool isInteger(const string_t& str);
  bool isFloatingPoint(const string_t& str);
}
}

#endif // TYPE_DETERMINE_UTILITY_H
