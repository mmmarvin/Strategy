#include "shape2dstyle.h"

namespace gengine2d
{
  Shape2DStyle::Shape2DStyle() :
    outlineThickness(0),
    outlineColor(Color::Black),
    fillColor(Color::White),
    texture(nullptr),
    textureRect(0, 0, 0, 0)
  {
  }
}
