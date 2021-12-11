#include "attackbasesystem.h"

#include "../../map/map.h"
#include "../../utility/position_utility.h"
#include "../../utility/steering_utility.h"
#include "../component/projectile_components.h"

namespace strat
{
namespace system
{
  AttackBaseSystem::AttackBaseSystem(EntityManager& entityManager,
                                     const Map& map,
                                     FogManager& fogs) :
    m_entityManager(&entityManager),
    m_map(&map),
    m_fogs(&fogs)
  {
  }

  void AttackBaseSystem::performAttack(const anax::Entity& entity,
                                       const anax::Entity& targetEntity,
                                       float attackDamage)
  {
    auto& targetAttribs = targetEntity.getComponent<component::BaseAttributes>().attributes;
    auto& targetHP = targetAttribs[component::BaseAttributes::BA_ATTRIB_HP];

    targetHP -= gengine2d::general_utils::min(targetHP, attackDamage);
    if(targetHP >= 1.f) {
      if(entity.hasComponent<component::ProjectileOwner>()) {
        trySelfDefence(entity.getComponent<component::ProjectileOwner>().owner, targetEntity);
      } else {
        trySelfDefence(entity, targetEntity);
      }
    } else {
      m_entityManager->scheduleForDeletion(targetEntity);
      if(base_component_utils::isOfType(entity,
                                        component::BaseEntityType::EEntityType::Projectile)) {
        const auto& ownerEntity = entity.getComponent<component::ProjectileOwner>().owner;
        if(ownerEntity.isValid()) {
          if(base_component_utils::isOfType(ownerEntity,
                                             component::BaseEntityType::EEntityType::Unit)) {
            unit_component_utils::setStateToIdle(entity);
            unit_component_utils::setTargetEntity(ownerEntity, anax::Entity());
          } else if(base_component_utils::isOfType(ownerEntity,
                                                   component::BaseEntityType::EEntityType::Building)) {
            unit_component_utils::setTargetEntity(ownerEntity, anax::Entity());
          }
        }
      } else {
        if(base_component_utils::isOfType(entity,
                                          component::BaseEntityType::EEntityType::Unit)) {
          unit_component_utils::setStateToIdle(entity);
          unit_component_utils::setTargetEntity(entity, anax::Entity());
        } else if(base_component_utils::isOfType(entity,
                                                 component::BaseEntityType::EEntityType::Building)) {
          unit_component_utils::setTargetEntity(entity, anax::Entity());
        }
      }
    }
  }

  void AttackBaseSystem::trySelfDefence(const anax::Entity& attacker,
                                        const anax::Entity& entity)
  {
    assert(!attacker.hasComponent<component::ProjectileOwner>());
    if(base_component_utils::isOfType(entity, component::BaseEntityType::EEntityType::Unit)) {
      if(unit_component_utils::getState(entity) == component::UnitState::EUnitState::Idle) {
        if(attacker.isValid()) {
          if(unit_component_utils::isSoldier(entity)) {
            auto team = unit_component_utils::getTeam(entity);
            const auto& attackerSpatialPosition = base_component_utils::getSpatialPositionHandle(attacker);

            if(m_fogs->getFog(team).isExposed(attackerSpatialPosition.getPosition().x,
                                          attackerSpatialPosition.getPosition().y)) {
              performSelfDefenceAttack(attacker, entity);
            } else {
              performSelfDefenceFlee(attacker, entity);
            }
          } else if(unit_component_utils::isWorker(entity)) {
            performSelfDefenceFlee(attacker, entity);
          }
        }
      }
    }
  }

  void AttackBaseSystem::performSelfDefenceFlee(const anax::Entity& attacker,
                                                const anax::Entity& entity)
  {
    const auto& position = base_component_utils::getPosition(entity);
    auto fleeDestination = gengine2d::normalize(position -
                                                base_component_utils::getPosition(attacker)) *
                                                (TILE_SIZE * 3.f);
    auto targetFleeDestination = position + fleeDestination;
    targetFleeDestination = position_utils::clampEntityPosition(targetFleeDestination,
                                                                base_component_utils::getBaseCollider(entity),
                                                                gengine2d::vector_utils::convertTo<float>(m_map->getSize()));

    unit_component_utils::setTargetPosition(entity, targetFleeDestination);
    unit_component_utils::setState(entity, component::UnitState::EUnitState::Moving);
  }

  void AttackBaseSystem::performSelfDefenceAttack(const anax::Entity& attacker,
                                                  const anax::Entity& entity)
  {
    unit_component_utils::setState(entity,
                                       component::UnitState::EUnitState::InitialMovingToAttack);
    unit_component_utils::setTargetEntity(entity, attacker);
  }
}
}
