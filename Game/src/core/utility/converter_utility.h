#ifndef CONVERTER_UTILITY_H
#define CONVERTER_UTILITY_H

#include "../type.h"

namespace gengine2d
{
namespace converter_utils
{
  template<class T> T stringTo(string_t str);
  template<class T> std::size_t to1D(T x, T y, T width);
}
}

#include "converter_utility.inl"

#endif // CONVERTER_UTILITY_H
