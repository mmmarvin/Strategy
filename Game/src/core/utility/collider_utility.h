#ifndef COLLIDER_UTILITY_H
#define COLLIDER_UTILITY_H

#include "../collider/rectangle.h"
#include "../vector2.h"

namespace gengine2d
{
namespace collider_utils
{
  template<class T>
  Rectangle<T> generateRect(const Vector2<T>& start,
                            const Vector2<T>& end);
}
}

#include "collider_utility.inl"

#endif // COLLIDER_UTILITY_H
