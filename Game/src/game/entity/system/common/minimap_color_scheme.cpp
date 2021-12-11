#include "minimap_color_scheme.h"

namespace strat
{
namespace system_common
{
  const gengine2d::vector_t<gengine2d::Color>& getMinimapColorScheme()
  {
    static const gengine2d::vector_t<gengine2d::Color> MINIMAP_ENTITY_COLORS =
    {
      gengine2d::Color::Blue,          // Player 1
      gengine2d::Color::Red,           // Player 2
      gengine2d::Color::Cyan,          // Player 3
      gengine2d::Color::Yellow,        // Player 4
      gengine2d::Color::Green,         // Player 5
      gengine2d::Color::Magenta,       // Player 6
      gengine2d::Color(128, 0, 128),   // Player 7
      gengine2d::Color(128, 64, 0),    // Player 8
      gengine2d::Color(255, 0, 255),   // Player 9
      gengine2d::Color::White          // Player 10
    };

    return MINIMAP_ENTITY_COLORS;
  }
}
}
