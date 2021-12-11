#include "gameworld.h"

namespace strat
{
  template<class PositionChangerFunc>
  void GameWorld::drawMiniMap(Player::EPlayerNumber team,
                              float xRatio,
                              float yRatio,
                              PositionChangerFunc&& positionChangerFunc) const
  {
    m_renderSystem.drawMiniMap(team,
                               xRatio,
                               yRatio,
                               std::forward<PositionChangerFunc>(positionChangerFunc));
  }
}
