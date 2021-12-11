#ifndef GAME_INTERNAL_TYPES_H
#define GAME_INTERNAL_TYPES_H

#include "input/sfml/sfmlinputhandler.h"
#include "renderer/sfml/sfmlrenderer.h"

namespace gengine2d
{
namespace game_internal
{
  // most renderers / and input handlers go hand-in-hand
  using Renderer = render::SFMLRenderer;
  using InputHandler = input::SFMLInputHandler;
}
}

#endif // GAME_INTERNAL_TYPES_H
