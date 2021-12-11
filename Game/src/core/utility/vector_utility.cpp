#include "vector_utility.h"

namespace gengine2d
{
namespace vector_utils
{
  Vector2f convertCartToIsoPoint(const Vector2f& point)
  {
    return Vector2f(point.x + point.y,
                    point.y - (point.x / 2.f));
  }

  Vector2f convertIsoToCartPoint(const Vector2f& point)
  {
    return Vector2f((point.x - point.y) / 1.5f,
                    (point.x / 3.f) + (point.y / 1.5f));
  }
}
}
