#ifndef VECTOR2_H
#define VECTOR2_H

#include <SFML/Graphics.hpp>

namespace gengine2d
{
  template<class T> using Vector2 = sf::Vector2<T>;

  template<class T> T magnitude2(const Vector2<T>& v);
  template<class T> T magnitude(const Vector2<T>& v);
  template<class T> T distance(const Vector2<T>& v1, const Vector2<T>& v2);
  template<class T> T dot(const Vector2<T>& v1, const Vector2<T>& v2);
  template<class T> Vector2<T> round(const Vector2<T>& v);
  template<class T> Vector2<T> normalize(Vector2<T> v);
  template<class T> Vector2<T> linterp(const Vector2<T>& oldPosition,
                                       const Vector2<T>& newPosition,
                                       const T& alpha);

  using Vector2f = Vector2<float>;
  using Vector2i = Vector2<int>;
  using Vector2u = Vector2<unsigned int>;
}

#include "vector2.inl"

#endif // VECTOR2_H
