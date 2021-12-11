#ifndef BUILDINGATTACKSYSTEM_H
#define BUILDINGATTACKSYSTEM_H

#include <anax/anax.hpp>
#include "../../../core/3rdparty/high_resolution_clock.h"
#include "../../spatialpartition.h"
#include "../component/base_components.h"
#include "../component/building_components.h"
#include "../component/unit_components.h"
#include "attackbasesystem.h"

namespace strat
{
  class EntityManager;
namespace system
{
  class BuildingAttackSystem : public anax::System<anax::Requires<component::BasePosition,
                                                                  component::BaseSpatialPosition,
                                                                  component::BaseCollider,
                                                                  component::BaseFlags,
                                                                  component::UnitTeam,
                                                                  component::UnitTarget,
                                                                  component::BuildingAttackAttributes,
                                                                  component::BuildingAttackTimer,
                                                                  component::BuildingCompleteness>>,
                               private AttackBaseSystem
  {
  public:
    explicit BuildingAttackSystem(EntityManager& entityManager,
                                  const Map& map,
                                  FogManager& fogs);

    void update();

  private:
    mar::high_resolution_clock  m_enemySearchTimer;
  };
}
}

#endif // BUILDINGATTACKSYSTEM_H
