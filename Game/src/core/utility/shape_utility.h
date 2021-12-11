#ifndef SHAPE_UTILITY_H
#define SHAPE_UTILITY_H

#include <array>
#include <tuple>
#include "../collider/rectangle.h"
#include "../vector2.h"

namespace gengine2d
{
namespace shape_utils
{
  template<class T>
  std::pair<Vector2<T>, Vector2<T>> quadToTightFitRect(const std::array<Vector2<T>, 4>& quadPoints);

  template<class T>
  std::array<Vector2<T>, 4> getRectPoints(const Rectangle<T>& rect);

  template<class T>
  std::array<T, 4> getRectPoints(const gengine2d::Vector2<T>& center,
                                 const gengine2d::Vector2<T>& size);
}
}

#include "shape_utility.inl"

#endif // SHAPE_UTILITY_H
