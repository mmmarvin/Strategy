#include "text2dstyle.h"

namespace gengine2d
{
  Text2DStyle::Text2DStyle() :
    characterSize(24),
    outlineThickness(0.f),
    outlineColor(Color::Black),
    fillColor(Color::White),
    font(nullptr),
    style(TS_REGULAR)
  {
  }
}
