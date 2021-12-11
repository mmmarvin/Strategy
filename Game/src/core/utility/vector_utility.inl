#include "vector_utility.h"

namespace gengine2d
{
namespace vector_utils
{
  template<class T, class F>
  Vector2<T> convertTo(const Vector2<F>& from)
  {
    static_assert(std::is_convertible<F, T>(), "Invalid types");
    return Vector2<T>(from.x, from.y);
  }
}
}
