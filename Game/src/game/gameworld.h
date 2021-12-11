#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "entity/system/buildingattacksystem.h"
#include "entity/system/buildingunittrainsystem.h"
#include "entity/system/projectilesystem.h"
#include "entity/system/rendersystem.h"
#include "entity/system/unitsystem.h"
#include "entitymanager.h"
#include "fogmanager.h"

namespace strat
{
  class GameWorld
  {
  public:
    GameWorld(const Map& map,
              FogManager& fogs,
              GlobalResource& globalResource);
    bool init();

    void draw(float alpha,
              bool drawShadow,
              bool drawHealth,
              bool drawDebugPosition,
              bool drawDebugBlastRadius) const;
    template<class PositionChangerFunc>
    void drawMiniMap(Player::EPlayerNumber team,
                     float xRatio,
                     float yRatio,
                     PositionChangerFunc&& positionChangerFunc) const;
    void update(float dt);

    EntityManager& getEntityManager() { return m_entityManager; }

  private:
    EntityManager                   m_entityManager;

    system::RenderSystem            m_renderSystem;
    system::UnitSystem              m_unitSystem;

    system::BuildingUnitTrainSystem m_buildingUnitTrainSystem;
    system::BuildingAttackSystem    m_buildingAttackSystem;

    system::ProjectileSystem        m_projectileSystem;
    system::ProjectileHomingSystem  m_projectileHomingSystem;
  };
}

#include "gameworld.inl"

#endif // GAMEWORLD_H
