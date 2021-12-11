#ifndef BUILDINGUNITTRAINSYSTEM_H
#define BUILDINGUNITTRAINSYSTEM_H

#include "../component/building_components.h"
#include "../component/unit_components.h"

namespace strat
{
  class EntityManager;
namespace system
{
  // TODO: Change to only update when building has unit to train
  class BuildingUnitTrainSystem : public anax::System<anax::Requires<component::UnitTeam,
                                                                     component::BuildingUnitTrainQueue>>
  {
  public:
    explicit BuildingUnitTrainSystem(EntityManager& entityManager);

    void update();

  private:
    EntityManager* m_entityManager;
  };
}
}

#endif // BUILDINGUNITTRAINSYSTEM_H
