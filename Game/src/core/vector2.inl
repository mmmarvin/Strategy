#include "vector2.h"

#include <cmath>

namespace gengine2d
{
  template<class T>
  T magnitude2(const Vector2<T>& v)
  {
      auto vx = v.x;
      auto vy = v.y;

      return (vx * vx) + (vy * vy);
  }

  template<class T>
  T magnitude(const Vector2<T>& v)
  {
      return std::sqrt(magnitude2(v));
  }

  template<class T>
  T distance(const Vector2<T>& v1, const Vector2<T>& v2)
  {
      T nx = v2.x - v1.x;
      T ny = v2.y - v1.y;

      return std::sqrt((nx * nx) + (ny * ny));
  }

  template<class T>
  T dot(const Vector2<T>& v1, const Vector2<T>& v2)
  {
      return (v1.x * v2.x) + (v1.y * v2.y);
  }

  template<class T>
  Vector2<T> round(const Vector2<T>& v)
  {
      return Vector2<T>(std::round(v.x), std::round(v.y));
  }

  template<class T>
  Vector2<T> normalize(Vector2<T> v)
  {
    auto length = magnitude(v);
    if(length > 0) {
        v /= length;
        return v;
    }

    return v;
  }

  template<class T> Vector2<T> linterp(const Vector2<T>& oldPosition,
                                       const Vector2<T>& newPosition,
                                       const T& alpha)
  {
    return (newPosition * alpha) + (oldPosition * (1.f - alpha));
  }
}
