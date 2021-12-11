#include "unitcommander.h"

#include "../../core/utility/vector_utility.h"
#include "../entity/component/base_components.h"
#include "../entitymanager.h"
#include "../player_action_acceptor.h"

namespace strat
{
  UnitCommander::UnitCommander(EntityManager& entityManager) :
    m_entityManager(&entityManager)
  {
  }

  void UnitCommander::setSelectedEntity(const anax::Entity& entity)
  {
    resetAndClearSelectedEntities();
    m_selectedEntities.push_back(entity);

    auto type = entity.getComponent<component::BaseEntityType>().type;
    if(entity.isValid() &&
       (type == component::BaseEntityType::EEntityType::Unit ||
        type == component::BaseEntityType::EEntityType::Building)) {
      entity.getComponent<component::BaseFlags>().flags[component::BaseFlags::BF_FLAG_IS_SELECTED] =  true;
    } else {
      m_selectedEntities.clear();
    }
  }

  void UnitCommander::setSelectedEntities(gengine2d::vector_t<anax::Entity> entities)
  {
    resetSelectedEntities();
    m_selectedEntities = std::move(entities);

    // select new entities
    for(std::size_t i = 0, isize = m_selectedEntities.size(); i < isize; ) {
      auto& entity = m_selectedEntities[i];
      if(entity.isValid() &&
         base_component_utils::isOfType(entity, component::BaseEntityType::EEntityType::Unit)) {
        entity.getComponent<component::BaseFlags>().flags[component::BaseFlags::BF_FLAG_IS_SELECTED] =  true;
        ++i;
      } else {
        if(entity != m_selectedEntities.back()) {
          std::swap(entity, m_selectedEntities.back());
        }
        m_selectedEntities.pop_back();
        isize = m_selectedEntities.size();
      }
    }
  }

  void UnitCommander::addToSelectedEntities(const anax::Entity& entity)
  {
    if(entity.isValid() &&
       base_component_utils::isOfType(entity, component::BaseEntityType::EEntityType::Unit)) {
      entity.getComponent<component::BaseFlags>().flags[component::BaseFlags::BF_FLAG_IS_SELECTED] =  true;
      m_selectedEntities.push_back(entity);
    }
  }

  void UnitCommander::resetAndClearSelectedEntities()
  {
    resetSelectedEntities();
    m_selectedEntities.clear();
  }

  void UnitCommander::commandToMoveSelectedEntities(const gengine2d::Vector2f& isoPosition)
  {
    if(isAllSelectedEntitiesUnits()) {
      commandSelectedEntities(component::UnitState::EUnitState::Moving,
                              anax::Entity(),
                              gengine2d::vector_utils::convertIsoToCartPoint(isoPosition),
                              [](const anax::Entity&) {});
    } else {
      if(m_selectedEntities.size() > 1) {
        resetAndClearSelectedEntities();
      }
    }
  }

  void UnitCommander::commandToMoveSelectedEntities(const anax::Entity& target)
  {
    if(isAllSelectedEntitiesUnits()) {
      if(target.isValid()) {
        switch(target.getComponent<component::BaseEntityType>().type) {
        case component::BaseEntityType::EEntityType::Building:
          commandSelectedEntities(component::UnitState::EUnitState::InitialMovingToBuild,
                                  target,
                                  target.getComponent<component::BasePosition>().position,
                                  [](const anax::Entity&){});
          break;
        case component::BaseEntityType::EEntityType::Props:
          break;
        case component::BaseEntityType::EEntityType::Resource:
          commandSelectedEntities(component::UnitState::EUnitState::InitialMovingToGather,
                                  target,
                                  target.getComponent<component::BasePosition>().position,
          [&target](const anax::Entity& entity) {
            if(entity.getComponent<component::UnitType>().type & component::UnitType::Worker) {
              auto resourceType = target.getComponent<component::ResourceType>().type;
              entity.getComponent<component::UnitWorkerResourceGathering>().resource = resourceType;
            }
          });
          break;
        case component::BaseEntityType::EEntityType::Unit:
          commandSelectedEntities(component::UnitState::EUnitState::InitialMovingToAttack,
                                  target,
                                  target.getComponent<component::BasePosition>().position,
                                  [](const anax::Entity&) {});
          break;
        default:
          break;
        }
      }
    } else {
      if(m_selectedEntities.size() > 1) {
        resetAndClearSelectedEntities();
      }
    }
  }

  void UnitCommander::commandToBuildSelectedEntities(const BuildingData& buildingData,
                                                     const gengine2d::Vector2f& isoPosition)
  {
    if(isAllSelectedEntitiesUnits()) {
      if(!m_selectedEntities.empty()) {
        // TODO: Change player number
        auto buildingEntity = action_acceptor::build(buildingData,
                                                     gengine2d::vector_utils::convertIsoToCartPoint(isoPosition),
                                                     Player::EPlayerNumber::PN_PLAYER1,
                                                     *m_entityManager);
        commandSelectedEntities(component::UnitState::EUnitState::InitialMovingToBuild,
                                buildingEntity,
                                buildingEntity.getComponent<component::BasePosition>().position,
                                [](const anax::Entity&){});
      }
    } else {
      if(m_selectedEntities.size() > 1) {
        resetAndClearSelectedEntities();
      }
    }
  }

  void UnitCommander::commandToIdleSelectedEntities()
  {
    if(isAllSelectedEntitiesUnits()) {
      if(!m_selectedEntities.empty()) {
        commandSelectedEntities(component::UnitState::EUnitState::Idle);
      }
    } else {
      if(m_selectedEntities.size() > 1) {
        resetAndClearSelectedEntities();
      }
    }
  }

  bool UnitCommander::isAllSelectedEntitiesUnits() const
  {
    return isAllEntitiesSelected([](const anax::Entity& entity) {
      return base_component_utils::isOfType(entity, component::BaseEntityType::EEntityType::Unit);
    });
  }

  void UnitCommander::resetSelectedEntities()
  {
    // unselect previous
    for(auto& entity : m_selectedEntities) {
      if(entity.isValid()) {
        entity.getComponent<component::BaseFlags>().flags[component::BaseFlags::BF_FLAG_IS_SELECTED] =  false;
      }
    }
  }

  template<class Func>
  void UnitCommander::commandSelectedEntities(component::UnitState::EUnitState state,
                                              const anax::Entity& target,
                                              const gengine2d::Vector2f& position,
                                              Func&& func)
  {
    for(std::size_t i = 0, isize = m_selectedEntities.size(); i < isize; ) {
      const auto& entity = m_selectedEntities[i];

      if(entity.isValid()) {
        if(state == component::UnitState::EUnitState::Idle) {
          unit_component_utils::setStateToIdle(entity);
        } else {
          unit_component_utils::setState(entity, state);
        }
        entity.getComponent<component::UnitTarget>().targetEntity = target;
        entity.getComponent<component::UnitTarget>().targetPosition = position;

        // stop all the timers
        if(entity.hasComponent<component::UnitSoldierAttackTimer>()) {
          entity.getComponent<component::UnitSoldierAttackTimer>().timer.stop();
          entity.getComponent<component::UnitSoldierWaitTimer>().timer.stop();
        }
        if(entity.hasComponent<component::UnitWorkerGatherTimer>()) {
          entity.getComponent<component::UnitWorkerGatherTimer>().timer.stop();
        }

        std::forward<Func>(func)(entity);
        ++i;
      } else {
        isize = gengine2d::container_utils::swapAndPop(m_selectedEntities, i);
      }
    }
  }

  void UnitCommander::commandSelectedEntities(component::UnitState::EUnitState state)
  {
    for(std::size_t i = 0, isize = m_selectedEntities.size(); i < isize; ) {
      const auto& entity = m_selectedEntities[i];

      if(entity.isValid()) {
        if(state == component::UnitState::EUnitState::Idle) {
          unit_component_utils::setStateToIdle(entity);
        } else {
          unit_component_utils::setState(entity, state);
        }

        // stop all the timers
        if(entity.hasComponent<component::UnitSoldierAttackTimer>()) {
          entity.getComponent<component::UnitSoldierAttackTimer>().timer.stop();
          entity.getComponent<component::UnitSoldierWaitTimer>().timer.stop();
        }
        if(entity.hasComponent<component::UnitWorkerGatherTimer>()) {
          entity.getComponent<component::UnitWorkerGatherTimer>().timer.stop();
        }
        ++i;
      } else {
        isize = gengine2d::container_utils::swapAndPop(m_selectedEntities, i);
      }
    }
  }
}
