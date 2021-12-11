#ifndef COLLISION_TEST_H
#define COLLISION_TEST_H

#include "circle.h"
#include "rectangle.h"

namespace gengine2d
{
  template<class T> bool contains(const Rectangle<T>& r, const Vector2<T>& v);
  template<class T> bool intersects(const Rectangle<T>& r1, const Rectangle<T>& r2);
  template<class T> bool fullyContains(const Rectangle<T>& r1, const Rectangle<T>& r2);

  template<class T> bool contains(const Circle<T>& c, const Vector2<T>& v);
  template<class T> bool intersects(const Circle<T>& c1, const Circle<T>& c2);
  template<class T> bool fullyContains(const Circle<T>& c1, const Circle<T>& c2);

  template<class T> bool fullyContains(const Rectangle<T>& r, const Circle<T>& c);
  template<class T> bool fullyContains(const Circle<T>& c, const Rectangle<T>& r);
}

#include "collision_test.inl"

#endif // COLLISION_TEST_H
