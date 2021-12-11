#include "collision_test.h"

#include "../debug/debug_log.h"

namespace gengine2d
{
  template<class T>
  bool contains(const Rectangle<T>& r, const Vector2<T>& v)
  {
    auto x1 = r.left;
    auto x2 = x1 + r.width;
    auto y1 = r.top;
    auto y2 = y1 + r.height;

    return v.x >= x1 && v.x <= x2 &&
           v.y >= y1 && v.y <= y2;
  }

  template<class T>
  bool intersects(const Rectangle<T>& r1, const Rectangle<T>& r2)
  {
    auto p1 = Vector2f(r2.left, r2.top);
    auto p2 = p1 + Vector2f(r2.width, 0);
    auto p3 = p2 + Vector2f(0, r2.height);
    auto p4 = p1 + Vector2f(0, r2.height);

    return contains(r1, p1) || contains(r1, p2) ||
           contains(r1, p3) || contains(r1, p4);
  }

  template<class T>
  bool fullyContains(const Rectangle<T>& r1, const Rectangle<T>& r2)
  {
    auto p1 = Vector2f(r2.left, r2.top);
    auto p2 = p1 + Vector2f(r2.width, 0);
    auto p3 = p2 + Vector2f(0, r2.height);
    auto p4 = p1 + Vector2f(0, r2.height);

    return contains(r1, p1) && contains(r1, p2) &&
           contains(r1, p3) && contains(r1, p4);
  }

  template<class T>
  bool contains(const Circle<T>& c, const Vector2<T>& v)
  {
    auto dist = distance(c.center, v);
    return dist <= c.radius;
  }

  template<class T>
  bool intersects(const Circle<T>& c1, const Circle<T>& c2)
  {
    auto diff = c1.center - c2.center;
    auto dist = dot(diff, diff);
    auto r = c1.radius + c2.radius;

    return dist <= (r * r);
  }

// TODO: Implement this
//  template<class T>
//  bool fullyContains(const Circle<T>& c1, const Circle<T>& c2)
//  {
//    return false;
//  }

  template<class T>
  bool fullyContains(const Rectangle<T>& r, const Circle<T>& c)
  {
    auto diameter = c.radius * 2;
    auto p = c.center - Vector2f(c.radius, c.radius);

    return fullyContains(r, Rectangle<T>(p.x, p.y, diameter, diameter));
  }

  template<class T>
  bool fullyContains(const Circle<T>& c, const Rectangle<T>& r)
  {
    auto p1 = Vector2f(r.left, r.top);
    auto p2 = p1 + Vector2f(r.width, 0);
    auto p3 = p2 + Vector2f(0, r.height);
    auto p4 = p1 + Vector2f(0, r.height);

    return contains(c, p1) && contains(c, p2) &&
           contains(c, p3) && contains(c, p4);
  }
}
