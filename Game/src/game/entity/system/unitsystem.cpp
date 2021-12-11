#include "unitsystem.h"

#include "../../../core/utility/general_utility.h"
#include "../../map/map.h"
#include "../../utility/position_utility.h"
#include "../../utility/steering_utility.h"
#include "../../utility/spatialpartition_utility.h"
#include "../../entitymanager.h"
#include "../../game_def.h"
#include "../component/building_components.h"
#include "../component/resource_components.h"

// TODO: Use helper functions
namespace strat
{
namespace system
{
namespace system_detail
{
  float getStopDistance(const anax::Entity& entity)
  {
    return entity.getComponent<component::BaseCollider>().collider.radius + UNIT_SIZE;
  }

  float getStopDistance(const anax::Entity& entity,
                        const anax::Entity& targetEntity)
  {
    return entity.getComponent<component::BaseCollider>().collider.radius +
           targetEntity.getComponent<component::BaseCollider>().collider.radius +
           UNIT_SIZE;
  }
}
  anax::Entity UnitSystem::CastleFinderSystem::findClosestCastle(const anax::Entity& entity) const
  {
    float closestDistance = std::numeric_limits<float>::max();
    anax::Entity closestEntity;

    if(entity.isValid()) {
      const auto& position = base_component_utils::getPosition(entity);

      for(const auto& otherEntity : this->getEntities()) {
        if(base_component_utils::isOfType(otherEntity, component::BaseEntityType::EEntityType::Building) &&
           unit_component_utils::sameTeam(entity, otherEntity) &&
           building_component_utils::isOfType(otherEntity, component::BuildingType::EBuildingType::BT_TYPE_CASTLE)) {
          const auto& otherPosition = base_component_utils::getPosition(otherEntity);

          auto d = gengine2d::distance(position, otherPosition);
          if(d <= closestDistance) {
            closestEntity = otherEntity;
            closestDistance = d;
          }
        }
      }
    }

    return closestEntity;
  }

  UnitSystem::UnitSystem(EntityManager& entityManager,
                         GlobalResource& globalResource,
                         const Map& map,
                         FogManager& fogs) :
    AttackBaseSystem(entityManager, map, fogs),
    m_globalResource(&globalResource),
    m_map(&map)
  {
  }

  void UnitSystem::update(float dt)
  {
    auto searchTimerElapsed = m_enemySearchTimer.get_elapsed().as_seconds();

    for(const auto& entity : this->getEntities()) {
      auto unitState = entity.getComponent<component::UnitState>().state;
      auto unitType = entity.getComponent<component::UnitType>().type;

      switch(unitState) {
      case component::UnitState::EUnitState::InitialMovingToAttack:
        initialMoveToAttack(entity);
        break;
      case component::UnitState::EUnitState::InitialMovingToGather:
        initialMoveToGather(entity);
        break;
      case component::UnitState::EUnitState::InitialMovingToBuild:
        initialMoveToBuild(entity);
        break;
      case component::UnitState::EUnitState::Moving:
        tryMove(entity, dt, system_detail::getStopDistance(entity));
        break;
      case component::UnitState::EUnitState::MovingToProximity:
        tryMove(entity, dt, TILE_SIZE);
        break;
      case component::UnitState::EUnitState::MovingToAttack:
        tryMoveToAttack(entity, dt);
        break;
      case component::UnitState::EUnitState::MovingToGather:
        tryMoveToGather(entity, dt);
        break;
      case component::UnitState::EUnitState::MovingToBuild:
        tryMoveToBuild(entity, dt);
        break;
      case component::UnitState::EUnitState::MovingToTarget:
        tryMoveToTarget(entity, dt);
        break;
      case component::UnitState::EUnitState::Idle:
        if(unitType & component::UnitType::EUnitType::Soldier) {
          // added delay to reduce CPU usage
          if(searchTimerElapsed >= 1.f) {
            AttackBaseSystem::trySearchForTarget(entity,
            [](const anax::Entity& e, const anax::Entity&) {
              unit_component_utils::setState(e, component::UnitState::EUnitState::MovingToAttack);
            });
          }
        }
        break;
      case component::UnitState::EUnitState::Attacking:
        tryAttack(entity);
        break;
      case component::UnitState::EUnitState::GatheringResource:
        tryGather(entity);
        break;
      case component::UnitState::EUnitState::ReturningResource:
        tryReturnResource(entity, dt);
        break;
      case component::UnitState::EUnitState::Building:
        tryBuild(entity);
        break;
      default:
        unit_component_utils::setStateToIdle(entity);
        break;
      }
    }

    if(searchTimerElapsed >= 1.f) {
      m_enemySearchTimer.restart();
    }
  }

  void UnitSystem::initialMoveToAttack(const anax::Entity& entity)
  {
    if(unit_component_utils::isWorker(entity)) {
      if(unit_component_utils::isWorkerCarryingResource(entity)) {
        unit_component_utils::setState(entity, component::UnitState::EUnitState::ReturningResource);
        return;
      }
    }

    // TODO: Check that the target entity is not invulnerable
    auto& unitTargetEntity = entity.getComponent<component::UnitTarget>().targetEntity;
    if(entity == unitTargetEntity) {
      unit_component_utils::setTargetEntity(entity, anax::Entity());
      unit_component_utils::setStateToIdle(entity);
    } else {
      if(base_component_utils::isValidOrReset(unitTargetEntity)) {
        if(unit_component_utils::isSoldier(entity) &&
           !unit_component_utils::sameTeam(entity, unitTargetEntity)) {
          unit_component_utils::setState(entity, component::UnitState::EUnitState::MovingToAttack);
        } else {
          unit_component_utils::setState(entity, component::UnitState::EUnitState::MovingToTarget);
        }
      } else {
        unit_component_utils::setState(entity, component::UnitState::EUnitState::Moving);
      }
    }
  }

  void UnitSystem::initialMoveToGather(const anax::Entity& entity)
  {
    auto isWorker = unit_component_utils::isWorker(entity);
    if(isWorker) {
      if(unit_component_utils::isWorkerCarryingResourceFull(entity)) {
        unit_component_utils::setState(entity, component::UnitState::EUnitState::ReturningResource);
        return;
      }
    }

    // TODO: Check if a different resource is currently being gathered than the target entity
    // if so, return the resource first
    auto& unitTargetEntity = entity.getComponent<component::UnitTarget>().targetEntity;
    if(entity == unitTargetEntity) {
      unit_component_utils::setTargetEntity(entity, anax::Entity());
      unit_component_utils::setStateToIdle(entity);
    } else {
      if(base_component_utils::isValidOrReset(unitTargetEntity)) {
        if(isWorker) {
          unit_component_utils::setState(entity, component::UnitState::EUnitState::MovingToGather);
        } else {
          unit_component_utils::setState(entity, component::UnitState::EUnitState::MovingToTarget);
        }
      } else {
        unit_component_utils::setState(entity, component::UnitState::EUnitState::Moving);
      }
    }
  }

  void UnitSystem::initialMoveToBuild(const anax::Entity& entity)
  {
    auto isWorker = unit_component_utils::isWorker(entity);
    if(isWorker) {
      if(unit_component_utils::isWorkerCarryingResource(entity)) {
        unit_component_utils::setState(entity, component::UnitState::EUnitState::ReturningResource);
        return;
      }
    }

    auto& unitTargetEntity = entity.getComponent<component::UnitTarget>().targetEntity;
    if(entity == unitTargetEntity) {
      unit_component_utils::setTargetEntity(entity, anax::Entity());
      unit_component_utils::setStateToIdle(entity);
    } else {
      if(base_component_utils::isValidOrReset(unitTargetEntity)) {
        if(unit_component_utils::isSoldier(entity)) {
          if(!unit_component_utils::sameTeam(entity, unitTargetEntity)) {
            unit_component_utils::setState(entity,
                                               component::UnitState::EUnitState::InitialMovingToAttack);
            return;
          }
        }

        if(isWorker) {
          if(unit_component_utils::sameTeam(entity, unitTargetEntity)) {
            unit_component_utils::setState(entity, component::UnitState::EUnitState::MovingToBuild);
          } else {
            unit_component_utils::setState(entity, component::UnitState::EUnitState::MovingToTarget);
          }
        } else {
          unit_component_utils::setState(entity, component::UnitState::EUnitState::MovingToTarget);
        }
      } else {
        unit_component_utils::setState(entity, component::UnitState::EUnitState::Moving);
      }
    }
  }

  void UnitSystem::tryMove(const anax::Entity& entity, float dt, float stopDistance)
  {
    performMoveToStopDistance(entity,
                              entity.getComponent<component::UnitTarget>().targetPosition,
                              stopDistance,
                              dt);
  }

  void UnitSystem::tryMoveToAttack(const anax::Entity& entity, float dt)
  {
    auto& unitTargetC = entity.getComponent<component::UnitTarget>();
    auto& unitTargetEntity = unitTargetC.targetEntity;

    assert(base_component_utils::isOfType(unitTargetEntity,
                                          component::BaseEntityType::EEntityType::Unit) ||
           base_component_utils::isOfType(unitTargetEntity,
                                          component::BaseEntityType::EEntityType::Building));

    if(base_component_utils::isValidOrReset(unitTargetEntity)) {
      // set the target's last known position to target position, so when the target entity
      // dies, we can still move to the last known position
      unitTargetC.targetPosition = unitTargetEntity.getComponent<component::BasePosition>().position;
      auto unitAttackRange = unit_component_utils::getSoldierAttackAttribute(entity,
                                                                  component::UnitSoldierAttackAttributes::UAA_ATTRIB_ATTACK_RANGE);
      performMoveToTarget(entity,
                          unitTargetEntity,
                          system_detail::getStopDistance(entity,
                                                         unitTargetEntity) + unitAttackRange,
                          component::UnitState::EUnitState::Attacking,
                          &UnitSystem::tryAttack,
                          dt);
    } else {
      unit_component_utils::setState(entity, component::UnitState::EUnitState::Moving);
      unitTargetEntity = anax::Entity();
    }
  }

  void UnitSystem::tryMoveToGather(const anax::Entity& entity, float dt)
  {
    auto& unitTargetC = entity.getComponent<component::UnitTarget>();
    auto& unitTargetEntity = entity.getComponent<component::UnitTarget>().targetEntity;

    if(base_component_utils::isValidOrReset(unitTargetEntity)) {
      unitTargetC.targetPosition = unitTargetEntity.getComponent<component::BasePosition>().position;
      performMoveToTarget(entity,
                          unitTargetEntity,
                          system_detail::getStopDistance(entity, unitTargetEntity),
                          component::UnitState::EUnitState::GatheringResource,
                          &UnitSystem::tryGather,
                          dt);
    } else {
      findClosestResourceToGatherResource(entity);
    }
  }

  void UnitSystem::tryMoveToBuild(const anax::Entity& entity, float dt)
  {
    auto& unitTargetEntity = entity.getComponent<component::UnitTarget>().targetEntity;
    if(base_component_utils::isValidOrReset(unitTargetEntity)) {
      if(!unitTargetEntity.getComponent<component::BuildingCompleteness>().completed) {
        performMoveToTarget(entity,
                            unitTargetEntity,
                            system_detail::getStopDistance(entity, unitTargetEntity),
                            component::UnitState::EUnitState::Building,
                            &UnitSystem::tryBuild,
                            dt);
      } else {
        unit_component_utils::setState(entity, component::UnitState::EUnitState::MovingToTarget);
        unit_component_utils::setTargetPosition(entity,
                                                    unitTargetEntity.getComponent<component::BasePosition>().position);
      }
    } else {
      unit_component_utils::setState(entity, component::UnitState::EUnitState::Moving);
      unitTargetEntity = anax::Entity();
    }
  }

  void UnitSystem::tryMoveToTarget(const anax::Entity& entity, float dt)
  {
    auto& unitTargetEntity = unit_component_utils::getTargetEntity(entity);
    if(base_component_utils::isValidOrReset(unitTargetEntity)) {
      performMoveToTarget(entity,
                          unitTargetEntity,
                          system_detail::getStopDistance(entity, unitTargetEntity),
                          component::UnitState::EUnitState::Idle,
                          nullptr,
                          dt);
    } else {
      unit_component_utils::setState(entity, component::UnitState::EUnitState::Moving);
      unitTargetEntity = anax::Entity();
    }
  }

  // TODO: Fix where unit still attacks entity even if he is out of range
  void UnitSystem::tryAttack(const anax::Entity& entity)
  {
    auto unitAttackSpeed = unit_component_utils::getSoldierAttackAttribute(entity,
                                                                    component::UnitSoldierAttackAttributes::UAA_ATTRIB_ATTACK_SPEED);
    auto unitAttackAnimationSpeed = unit_component_utils::getSoldierAttackAttribute(entity,
                                                                             component::UnitSoldierAttackAttributes::UAA_ATTRIB_ATTACK_ANIMATION_SPEED);

    auto& unitAttackTimer = entity.getComponent<component::UnitSoldierAttackTimer>().timer;
    auto& unitWaitTimer = entity.getComponent<component::UnitSoldierWaitTimer>().timer;

    auto& unitTargetC = entity.getComponent<component::UnitTarget>();
    auto& unitTargetEntity = unitTargetC.targetEntity;

    assert(base_component_utils::isOfType(unitTargetEntity,
                                          component::BaseEntityType::EEntityType::Unit) ||
           base_component_utils::isOfType(unitTargetEntity,
                                          component::BaseEntityType::EEntityType::Building));

    if(base_component_utils::isValidOrReset(unitTargetEntity)) {
      if(unitAttackTimer.is_running()) {
        if(unitAttackTimer.get_elapsed().as_seconds() >= unitAttackAnimationSpeed) {
          auto attackType = entity.getComponent<component::UnitSoldierAttackType>().type;
          if(attackType == component::UnitSoldierAttackType::EAttackType::AT_TYPE_MELEE) {
            performAttack(entity);
          } else {
            m_entityManager->createProjectile(entity,
                                              unit_component_utils::getTeam(entity),
                                              base_component_utils::getPosition(entity),
                                              base_component_utils::getPosition(unitTargetEntity));
          }

          const auto& position = entity.getComponent<component::BasePosition>().position;
          const auto& targetPosition = unitTargetEntity.getComponent<component::BasePosition>().position;
          auto unitAttackRange = unit_component_utils::getSoldierAttackAttribute(entity,
                                                                          component::UnitSoldierAttackAttributes::UAA_ATTRIB_ATTACK_RANGE);
          if(gengine2d::distance(position, targetPosition) >
             system_detail::getStopDistance(entity, unitTargetEntity) + unitAttackRange) {
            // set the target's last known position to target position, so when the target entity
            // dies, we can still move to the last known position
            unit_component_utils::setState(entity, component::UnitState::EUnitState::MovingToAttack);
            unitTargetC.targetPosition = unitTargetEntity.getComponent<component::BasePosition>().position;
          }

          unitAttackTimer.stop();
        }
      } else {
        if(unitWaitTimer.is_running()) {
          if(unitWaitTimer.get_elapsed().as_seconds() >= unitAttackSpeed) {
            unitAttackTimer.start();
            unitWaitTimer.stop();
          }
        } else {
          unitWaitTimer.start();
        }
      }
    } else {
      unit_component_utils::setStateToIdle(entity);
    }
  }

  void UnitSystem::tryGather(const anax::Entity& entity)
  {
    auto& unitTargetC = entity.getComponent<component::UnitTarget>();
    auto& unitTargetEntity = unitTargetC.targetEntity;

    if(base_component_utils::isValidOrReset(unitTargetEntity)) {
      assert(unitTargetEntity.hasComponent<component::ResourceType>());

      auto& unitGatherTimer = entity.getComponent<component::UnitWorkerGatherTimer>().timer;
      if(unitGatherTimer.is_running()) {
        if(unitGatherTimer.get_elapsed().as_seconds() >= 2.5f) {
          auto& unitCollectedResource = entity.getComponent<component::UnitWorkerResourceCollected>().collected;
          auto resourceType = unitTargetEntity.getComponent<component::ResourceType>().type;
          auto availableResource = resource_component_utils::getAttribute(unitTargetEntity,
                                                                          component::ResourceAttributes::RA_ATTRIB_RES_VALUE_LEFT);

          assert(resourceType < GlobalResource::GR_NUMBER_OF_RES);

          // TODO: Make how many resource collected dependent on the unit
          auto valueToTake = gengine2d::general_utils::min(availableResource, 5);
          unitCollectedResource[resourceType] += valueToTake;
          resource_component_utils::setAttribute(unitTargetEntity,
                                                 component::ResourceAttributes::RA_ATTRIB_RES_VALUE_LEFT,
                                                 availableResource - valueToTake);

          // TODO: Make limit to how much resource a unit can carry dependent
          // on the unit
          if(unitCollectedResource[resourceType] >= 10) {
            unitGatherTimer.stop();

            unit_component_utils::setState(entity, component::UnitState::EUnitState::ReturningResource);
          } else {
            unitGatherTimer.restart();
          }
        }
      } else {
        unitGatherTimer.start();
      }
    } else {
      findClosestResourceToGatherResource(entity);
    }
  }

  // TODO: Make initial returning resource to be able to choose closest castle to return
  // to
  void UnitSystem::tryReturnResource(const anax::Entity& entity, float dt)
  {
    auto& unitCastle = entity.getComponent<component::UnitWorkerCastle>().castle;
    if(base_component_utils::isValidOrReset(unitCastle)) {
      performMoveToTarget(entity,
                          unitCastle,
                          system_detail::getStopDistance(entity, unitCastle),
                          component::UnitState::EUnitState::MovingToGather,
                          &UnitSystem::performReturnResource,
                          dt);
    } else {
      auto closestCastle = m_castleFinderSystem.findClosestCastle(entity);
      if(closestCastle.isValid()) {
        unitCastle = closestCastle;
        performMoveToTarget(entity,
                            unitCastle,
                            system_detail::getStopDistance(entity, unitCastle),
                            component::UnitState::EUnitState::MovingToGather,
                            &UnitSystem::performReturnResource,
                            dt);
      } else {
        unit_component_utils::setStateToIdle(entity);
      }
    }
  }

  void UnitSystem::tryBuild(const anax::Entity& entity)
  {
    auto& unitTargetEntity = entity.getComponent<component::UnitTarget>().targetEntity;
    auto& targetAttribs = unitTargetEntity.getComponent<component::BaseAttributes>().attributes;
    auto& targetHpAttrib = targetAttribs[component::BaseAttributes::BA_ATTRIB_HP];
    auto& targetHpMaxAttrib = targetAttribs[component::BaseAttributes::BA_ATTRIB_MAX_HP];

    if(targetHpAttrib < targetHpMaxAttrib) {
      auto& buildTimer = entity.getComponent<component::UnitWorkerGatherTimer>().timer;

      if(buildTimer.is_running()) {
        if(buildTimer.get_elapsed().as_seconds() >= 5.f) {
          targetHpAttrib = gengine2d::general_utils::min(targetHpAttrib + 1.f, targetHpMaxAttrib);
          if(targetHpAttrib >= targetHpMaxAttrib) {
            unit_component_utils::setStateToIdle(entity);
            unitTargetEntity.getComponent<component::BuildingCompleteness>().completed = true;
            unitTargetEntity = anax::Entity();
          }

          buildTimer.stop();
        }
      } else {
        buildTimer.start();
      }
    } else {
      unit_component_utils::setStateToIdle(entity);
    }
  }

  void UnitSystem::performMove(const anax::Entity& entity,
                               const gengine2d::Vector2f& targetPosition,
                               float dt)
  {
    auto& positionC = entity.getComponent<component::BasePosition>();
    auto& oldPosition = positionC.oldPosition;
    auto& position = positionC.position;
    auto& spatialPosition = entity.getComponent<component::BaseSpatialPosition>().spatialPosition;
    auto moveSpeed = unit_component_utils::getAttribute(entity,
                                                        component::UnitAttributes::UA_ATTRIB_MOVE_SPEED);

    oldPosition = position;
    float adjustedMoveSpeed = moveSpeed * dt;

    // TODO: Implement more efficient collision detection
    gengine2d::Vector2f vel;
    auto separationForce = steering_utils::separate(entity,
                                                    position,
                                                    vel,
                                                    adjustedMoveSpeed,
                                                    m_entityManager->getSpatialPartition());
    if(steering_utils::accumulateForce(vel, separationForce, adjustedMoveSpeed)) {
      auto seekForce = steering_utils::seek(position,
                                            targetPosition,
                                            vel,
                                            adjustedMoveSpeed);
      steering_utils::accumulateForce(vel, seekForce, adjustedMoveSpeed);
    }

    position += vel;
    position = position_utils::clampEntityPosition(position,
                                                   base_component_utils::getBaseCollider(entity),
                                                   gengine2d::vector_utils::convertTo<float>(m_map->getSize()));
    spatialpartition_utils::updateSpatialPosition(entity,
                                                  position,
                                                  spatialPosition,
                                                  m_entityManager->getSpatialPartition());
    m_fogs->getFog(unit_component_utils::getTeam(entity)).exposeRadius(spatialPosition.getPosition().x,
                                                                       spatialPosition.getPosition().y,
                                                                       1.f);
  }

  void UnitSystem::performMoveToStopDistance(const anax::Entity& entity,
                                             const gengine2d::Vector2f& targetPosition,
                                             float stopDistance,
                                             float dt)
  {

    const auto& position = entity.getComponent<component::BasePosition>().position;

    if(gengine2d::distance(position, targetPosition) >= stopDistance) {
      performMove(entity, targetPosition, dt);
    } else {
      unit_component_utils::setStateToIdle(entity);
    }
  }

  void UnitSystem::performAttack(const anax::Entity& entity)
  {
    AttackBaseSystem::performAttack(entity,
                                    entity.getComponent<component::UnitTarget>().targetEntity,
                                    unit_component_utils::getSoldierAttackAttribute(entity,
                                                                             component::UnitSoldierAttackAttributes::UAA_ATTRIB_ATTACK_DAMAGE));
  }

  void UnitSystem::performReturnResource(const anax::Entity& entity)
  {
    auto& collection = entity.getComponent<component::UnitWorkerResourceCollected>().collected;
    for(std::size_t i = 0, isize = collection.capacity(); i < isize; ++i) {
      auto& resource = collection[i];
      if(resource > 0) {
        assert(i < GlobalResource::EGlobalResouceType::GR_NUMBER_OF_RES);
        m_globalResource->addResource(entity.getComponent<component::UnitTeam>().team,
                                      static_cast<GlobalResource::EGlobalResouceType>(i),
                                      resource);
        resource = 0;
      }
    }

    auto resourceCollecting = entity.getComponent<component::UnitWorkerResourceGathering>().resource;
    if(resourceCollecting == -1) {
      unit_component_utils::setStateToIdle(entity);
    } else {
      auto& unitTargetEntity = entity.getComponent<component::UnitTarget>().targetEntity;
      if(base_component_utils::isValidOrReset(unitTargetEntity)) {
        switch(unitTargetEntity.getComponent<component::BaseEntityType>().type) {
        case component::BaseEntityType::EEntityType::Resource:
          unit_component_utils::setState(entity,
                                             component::UnitState::EUnitState::InitialMovingToGather);
          unit_component_utils::setTargetPosition(entity,
                                                      unitTargetEntity.getComponent<component::BasePosition>().position);
          break;

        case component::BaseEntityType::EEntityType::Unit:
          unit_component_utils::setState(entity,
                                             component::UnitState::EUnitState::InitialMovingToAttack);
          unit_component_utils::setTargetPosition(entity,
                                                      unitTargetEntity.getComponent<component::BasePosition>().position);
          break;

        case component::BaseEntityType::EEntityType::Building:
          unit_component_utils::setState(entity,
                                             component::UnitState::EUnitState::InitialMovingToBuild);
          unit_component_utils::setTargetPosition(entity,
                                                      unitTargetEntity.getComponent<component::BasePosition>().position);
          break;

        default:
          unit_component_utils::setStateToIdle(entity);
          break;
        }
      } else {
        unit_component_utils::setStateToIdle(entity);
      }
    }
  }

  void UnitSystem::performMoveToTarget(const anax::Entity& entity,
                                   const anax::Entity& target,
                                   float stopDistance,
                                   component::UnitState::EUnitState finalState,
                                   void (UnitSystem::*rangeReachedEvent)(const anax::Entity&),
                                   float dt)
  {
    const auto& position = entity.getComponent<component::BasePosition>().position;
    const auto& targetPosition = target.getComponent<component::BasePosition>().position;

    if(gengine2d::distance(position, targetPosition) <= stopDistance) {
      entity.getComponent<component::BasePosition>().oldPosition = position;
      unit_component_utils::setState(entity, finalState);
      if(rangeReachedEvent)
        (this->*rangeReachedEvent)(entity);
    } else {
      performMove(entity,
                  target.getComponent<component::BasePosition>().position,
                  dt);
    }
  }

  void UnitSystem::findClosestResourceToGatherResource(const anax::Entity& entity)
  {
    auto& unitTargetEntity = entity.getComponent<component::UnitTarget>().targetEntity;

    // NOTE: Set resource gathering whenever a entity is clicked to remember which resource
    // we want to gather
    auto resourceGathering = entity.getComponent<component::UnitWorkerResourceGathering>().resource;
    assert(resourceGathering != -1);

    // if the entity is no longer alive, we search for a close entity
    // that is the same resource as the last target
    auto [spx, spy] = entity.getComponent<component::BaseSpatialPosition>().spatialPosition.getPosition();
    auto closestResource = spatialpartition_utils::findEntityExpandingFromPoint(spx,
                                                                                spy,
                                                                                5.f,
                                                                                m_entityManager->getSpatialPartition(),
    [&entity, resourceGathering](const anax::Entity& otherEntity) {
      return (entity != otherEntity &&
              base_component_utils::isOfType(otherEntity,
                                             component::BaseEntityType::EEntityType::Resource) &&
              static_cast<int>(otherEntity.getComponent<component::ResourceType>().type) == resourceGathering);
    });

    if(closestResource.isValid()) {
      unitTargetEntity = closestResource;
      unit_component_utils::setState(entity, component::UnitState::EUnitState::MovingToGather);
      unit_component_utils::setTargetPosition(entity,
                                                  unitTargetEntity.getComponent<component::BasePosition>().position);
    } else {
      if(unit_component_utils::isWorkerCarryingResource(entity)) {
        unit_component_utils::setState(entity,
                                           component::UnitState::EUnitState::ReturningResource);
        unitTargetEntity = anax::Entity();
      } else {
        unit_component_utils::setStateToIdle(entity);
      }

      entity.getComponent<component::UnitWorkerResourceGathering>().resource = -1;
    }
  }
}
}
