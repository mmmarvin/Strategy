#ifndef VERTEX2_H
#define VERTEX2_H

#include "../../color.h"
#include "../../vector2.h"

namespace gengine2d
{
  struct Vertex2
  {
    Vertex2();
    Vertex2(const Vector2f& position_,
            const Color& color_,
            const Vector2f& textureCoordinate_);

    Vector2f  position;
    Color     color;
    Vector2f  textureCoordinate;
  };
}

#endif // VERTEX2_H
