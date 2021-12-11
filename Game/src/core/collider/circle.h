#ifndef CIRCLE_H
#define CIRCLE_H

#include "../vector2.h"

namespace gengine2d
{
  template<class T>
  class Circle
  {
  public:
    Circle() : center(0, 0), radius(0) {}
    Circle(T x, T y, T r) : center(x, y), radius(r) {}

    Vector2<T>  center;
    T           radius;
  };

  using Circlef = Circle<float>;
  using Circlei = Circle<int>;
  using Circleu = Circle<unsigned int>;
}

#endif // CIRCLE_H
