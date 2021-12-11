#ifndef MINIMAP_COLOR_SCHEME_H
#define MINIMAP_COLOR_SCHEME_H

#include <SFML/Graphics.hpp>
#include "../../../core/color.h"
#include "../../../core/type.h"

namespace strat
{
namespace system_common
{
  const gengine2d::vector_t<gengine2d::Color>& getMinimapColorScheme();
}
}

#endif // MINIMAP_COLOR_SCHEME_H
