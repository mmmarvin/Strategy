#include "shape_utility.h"

namespace gengine2d
{
namespace shape_utils
{
  template<class T>
  std::pair<Vector2<T>, Vector2<T>> quadToTightFitRect(const std::array<Vector2<T>, 4>& quadPoints)
  {
    auto xs = { quadPoints[0].x, quadPoints[1].x, quadPoints[2].x, quadPoints[3].x };
    auto ys = { quadPoints[0].y, quadPoints[1].y, quadPoints[2].y, quadPoints[3].y };

    float minX = *std::min_element(xs.begin(), xs.end());
    float maxX = *std::max_element(xs.begin(), xs.end());
    float minY = *std::min_element(ys.begin(), ys.end());
    float maxY = *std::max_element(ys.begin(), ys.end());

    return std::make_pair(Vector2<T>(minX, minY), Vector2<T>(maxX, maxY));
  }

  template<class T>
  std::array<Vector2<T>, 4> getRectPoints(const Rectangle<T>& rect)
  {
    std::array<Vector2<T>, 4> ret;

    auto l = rect.left, t = rect.top, w = rect.width, h = rect.height;
    auto lw = l + w, th = t + h;

    ret[0] = Vector2f(l, t);
    ret[1] = Vector2f(lw, t);
    ret[2] = Vector2f(lw, th);
    ret[3] = Vector2f(l, th);

    return ret;
  }

  template<class T>
  std::array<T, 4> getRectPoints(const gengine2d::Vector2<T>& center,
                                 const gengine2d::Vector2<T>& size)
  {
    auto drawPos = center - (size / 2.f);
    return std::array<T, 4>{ drawPos.x, drawPos.y, drawPos.x + size.x, drawPos.y + size.y };
  }
}
}
