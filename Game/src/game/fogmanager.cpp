#include "fogmanager.h"

namespace strat
{
  bool FogManager::init(std::size_t columnCount, std::size_t rowCount)
  {
    for(auto& fog : m_fogs) {
      if(!fog.init(columnCount, rowCount)) {
        return false;
      }
    }

    return true;
  }

  void FogManager::draw(Player::EPlayerNumber team)
  {
    m_fogs[team].draw();
  }

  Fog& FogManager::getFog(Player::EPlayerNumber team)
  {
    return m_fogs[team];
  }

  const Fog& FogManager::getFog(Player::EPlayerNumber team) const
  {
    return m_fogs[team];
  }
}
