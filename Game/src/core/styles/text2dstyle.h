#ifndef TEXT2DSTYLE_H
#define TEXT2DSTYLE_H

#include "../../color.h"
#include "../../game_internal_types.h"

namespace gengine2d
{
  struct Text2DStyle
  {
    enum ETextStyle : unsigned char
    {
      TS_REGULAR = 1 << 0,
      TS_BOLD = 1 << 1,
      TS_ITALIC = 1 << 2
    };

    Text2DStyle();

    unsigned int                          characterSize;
    float                                 outlineThickness;
    Color                                 outlineColor;
    Color                                 fillColor;
    const game_internal::Renderer::Font*  font;
    ETextStyle                            style;
  };
}

#endif // TEXT2DSTYLE_H
