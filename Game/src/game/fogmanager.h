#ifndef FOGMANAGER_H
#define FOGMANAGER_H

#include "../core/3rdparty/fixed_sized_array.h"
#include "fog.h"
#include "player.h"

namespace strat
{
  class FogManager
  {
  public:
    FogManager() {}
    bool init(std::size_t columnCount, std::size_t rowCount);

    void draw(Player::EPlayerNumber team);

    Fog& getFog(Player::EPlayerNumber team);
    const Fog& getFog(Player::EPlayerNumber team) const;

  private:
    mar::container::fixed_sized_array<Fog, Player::EPlayerNumber::PN_MAX_PLAYER_NUMBER> m_fogs;
  };
}

#endif // FOGMANAGER_H
