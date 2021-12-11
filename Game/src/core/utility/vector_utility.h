#ifndef VECTOR_UTILITY_H
#define VECTOR_UTILITY_H

#include "../vector2.h"

namespace gengine2d
{
namespace vector_utils
{
  Vector2f convertCartToIsoPoint(const Vector2f& point);
  Vector2f convertIsoToCartPoint(const Vector2f& point);
  template<class T, class F> Vector2<T> convertTo(const Vector2<F>& from);
}
}

#include "vector_utility.inl"

#endif // VECTOR_UTILITY_H
