#include "attackbasesystem.h"

#include "../../utility/spatialpartition_utility.h"
#include "../component/base_components.h"
#include "../component/unit_components.h"

namespace strat
{
namespace system
{
  template<class Func>
  bool AttackBaseSystem::trySearchForTarget(const anax::Entity& entity,
                                            Func&& func)
  {
    // TODO: Make more efficient searching like search in groups
    const auto& position = entity.getComponent<component::BasePosition>().position;

    auto [spx, spy]  = entity.getComponent<component::BaseSpatialPosition>().spatialPosition.getPosition();

    // TODO: Update search size to be specific
    const auto* fogs = m_fogs;
    auto closestEntity = spatialpartition_utils::findClosestEntity(spx - 5.f,
                                                                   spy - 5.f,
                                                                   spx + 5.f,
                                                                   spy + 5.f,
                                                                   position,
                                                                   m_entityManager->getSpatialPartition(),
    [&entity, &fogs](const anax::Entity& otherEntity) {
      if(entity != otherEntity &&
         (base_component_utils::isOfType(otherEntity,
                                         component::BaseEntityType::EEntityType::Unit) ||
          base_component_utils::isOfType(otherEntity,
                                         component::BaseEntityType::EEntityType::Building)) &&
          !unit_component_utils::sameTeam(entity, otherEntity)) {
        auto team = unit_component_utils::getTeam(entity);
        const auto& otherSpatialPosition = base_component_utils::getSpatialPositionHandle(otherEntity);

        return fogs->getFog(team).isExposed(otherSpatialPosition.getPosition().x,
                                        otherSpatialPosition.getPosition().y);
      }

      return false;
    });
    if(closestEntity.isValid()) {
      unit_component_utils::setTargetEntity(entity, closestEntity);
      std::forward<Func>(func)(entity, closestEntity);

      return true;
    }

    return false;
  }
}
}
