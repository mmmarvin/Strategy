#include "position_utility.h"

#include "../../core/utility/general_utility.h"
#include "../game_def.h"

namespace strat
{
namespace position_utils
{
  gengine2d::Vector2f clampEntityPosition(const gengine2d::Vector2f& position,
                                          const gengine2d::Circlef& collider,
                                          const gengine2d::Vector2f& mapSize)
  {
    auto adjustedPosition = position - collider.center;
    adjustedPosition.x = gengine2d::general_utils::clamp(adjustedPosition.x,
                                                         collider.radius,
                                                         (mapSize.x * TILE_SIZE) - collider.radius);
    adjustedPosition.y = gengine2d::general_utils::clamp(adjustedPosition.y,
                                                         collider.radius,
                                                         (mapSize.y * TILE_SIZE) - collider.radius);

    return adjustedPosition;
  }
}
}
