#include "collider_utility.h"

#include "general_utility.h"

namespace gengine2d
{
namespace collider_utils
{
  template<class T>
  Rectangle<T> generateRect(const Vector2<T>& start,
                            const Vector2<T>& end)
  {
    auto dim = end - start;
    return Rectangle<T>(start.x, start.y, dim.x, dim.y);
  }
}
}
