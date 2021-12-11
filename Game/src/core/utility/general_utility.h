#ifndef GENERAL_UTILITY_H
#define GENERAL_UTILITY_H

#include <tuple>
#include "../vector2.h"

namespace gengine2d
{
namespace general_utils
{
  template<class T>
  std::pair<T, T> minmax(T a, T b);

  template<class T>
  T clamp(T value, T min, T max);

  template<class T>
  T min(T a, T b);

  template<class T>
  T max(T a, T b);

  template<class T>
  T fast_abs(T t);

  template<class T>
  T fast_mod(T a, T b);

  template<class T>
  bool is_within_ex(T value, T min, T max);

  template<class T>
  bool is_within_in(T value, T min, T max);

  template<class T>
  bool is_within_lin(T value, T min, T max);

  template<class T>
  bool is_within_rin(T value, T min, T max);

  template<class T, class S>
  T* static_cast_to_ptr_type(S* v);

  template<class T, class S>
  const T* static_cast_to_ptr_type(const S* v);
}
}

#include "general_utility.inl"

#endif // GENERAL_UTILITY_H
