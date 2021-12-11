#include "buildingunittrainsystem.h"

#include "../../entitymanager.h"

namespace strat
{
namespace system
{
  BuildingUnitTrainSystem::BuildingUnitTrainSystem(EntityManager& entityManager) :
    m_entityManager(&entityManager)
  {
  }

  void BuildingUnitTrainSystem::update()
  {
    for(const auto& entity : this->getEntities()) {
      auto& unitTrainQueue = entity.getComponent<component::BuildingUnitTrainQueue>().queue;
      if(!unitTrainQueue.empty()) {
        auto& [unitDataToTrain, trainTimer] = unitTrainQueue.front();
        if(trainTimer.is_running()) {
          if(trainTimer.get_elapsed().as_seconds() >= unitDataToTrain.get().creationRequirements.creationTime) {
            // TODO: Change castle to be empty?
            auto entityCreated = m_entityManager->createUnit(unitDataToTrain.get(),
                                                             unit_component_utils::getTeam(entity),
                                                             base_component_utils::getPosition(entity),
                                                             entity);
            const auto& targetPosition = building_component_utils::getTrainTargetPosition(entity);
            auto& targetEntity = building_component_utils::getTrainTargetEntity(entity);
            if(base_component_utils::isValidOrReset(targetEntity)) {
              unit_component_utils::setTargetEntity(entityCreated, targetEntity);
              switch(targetEntity.getComponent<component::BaseEntityType>().type) {
              case component::BaseEntityType::EEntityType::Building:
                unit_component_utils::setState(entityCreated,
                                                   component::UnitState::EUnitState::InitialMovingToBuild);
                break;
              case component::BaseEntityType::EEntityType::Resource:
                entityCreated.getComponent<component::UnitWorkerResourceGathering>().resource = resource_component_utils::getResourceType(targetEntity);
                unit_component_utils::setState(entityCreated,
                                                   component::UnitState::EUnitState::InitialMovingToGather);
                break;
              case component::BaseEntityType::EEntityType::Unit:
                unit_component_utils::setState(entityCreated,
                                                   component::UnitState::EUnitState::InitialMovingToAttack);
                break;
              default:
                break;
              }
            } else if(targetPosition.x > 0.f && targetPosition.y > 0.f) {
              unit_component_utils::setTargetPosition(entityCreated, targetPosition);
              unit_component_utils::setState(entityCreated,
                                                 component::UnitState::EUnitState::MovingToProximity);
            }
            unitTrainQueue.erase(unitTrainQueue.begin());
          }
        } else {
          trainTimer.restart();
        }
      }
    }
  }
}
}
