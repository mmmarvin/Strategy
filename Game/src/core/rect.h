#ifndef RECT_H
#define RECT_H

#include <SFML/Graphics/Rect.hpp>

namespace gengine2d
{
  template<class T> using Rect = sf::Rect<T>;
  using Rectf = Rect<float>;
  using Recti = Rect<int>;
  using Rectu = Rect<unsigned int>;
}

#endif // RECT_H
