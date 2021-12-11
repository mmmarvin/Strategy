#include "vertex2.h"

namespace gengine2d
{
  Vertex2::Vertex2() :
    position(0, 0),
    color(255, 255, 255, 255),
    textureCoordinate(0, 0)
  {
  }

  Vertex2::Vertex2(const Vector2f& position_,
                   const Color& color_,
                   const Vector2f& textureCoordinate_) :
    position(position_),
    color(color_),
    textureCoordinate(textureCoordinate_)
  {
  }
}
