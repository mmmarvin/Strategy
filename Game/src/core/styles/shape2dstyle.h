#ifndef SHAPESTYLE_H
#define SHAPESTYLE_H

#include "../../color.h"
#include "../../game_internal_types.h"

namespace gengine2d
{
  struct Shape2DStyle
  {
    Shape2DStyle();

    render::position_point_2d_t                 outlineThickness;
    Color                                       outlineColor;
    Color                                       fillColor;
    const game_internal::Renderer::Texture*     texture;
    Rect<render::position_point_2d_t>           textureRect;
  };
}

#endif // SHAPESTYLE_H
