#include "buildingattacksystem.h"

#include "../../game_def.h"
#include "../../entitymanager.h"

namespace strat
{
namespace system
{
  BuildingAttackSystem::BuildingAttackSystem(EntityManager& entityManager,
                                             const Map& map,
                                             FogManager& fogs) :
    AttackBaseSystem(entityManager, map, fogs)
  {
  }

  void BuildingAttackSystem::update()
  {
    auto searchTimer = m_enemySearchTimer.get_elapsed().as_seconds();

    for(const auto& entity : this->getEntities()) {
      auto attackType = building_component_utils::getAttackType(entity);
      if(entity.getComponent<component::BuildingCompleteness>().completed &&
         attackType != component::BuildingAttackType::EAttackType::AT_TYPE_NONE) {
        auto& targetEntity = entity.getComponent<component::UnitTarget>().targetEntity;
        if(base_component_utils::isValidOrReset(targetEntity)) {
          const auto battribs = entity.getComponent<component::BuildingAttackAttributes>().attributes;
          auto& attackTimer = entity.getComponent<component::BuildingAttackTimer>().timer;
          auto attackSpeed = battribs[component::BuildingAttackAttributes::BUA_ATTRIB_ATTACK_SPEED];

          if(attackTimer.get_elapsed().as_seconds() >= attackSpeed) {
            auto d = gengine2d::distance(targetEntity.getComponent<component::BasePosition>().position,
                                         entity.getComponent<component::BasePosition>().position);
            if(d <= battribs[component::BuildingAttackAttributes::BUA_ATTRIB_ATTACK_RANGE]) {
              if(attackType == component::BuildingAttackType::EAttackType::AT_TYPE_MELEE) {
                AttackBaseSystem::performAttack(entity,
                                                targetEntity,
                                                battribs[component::BuildingAttackAttributes::BUA_ATTRIB_ATTACK_DAMAGE]);
              } else if(attackType == component::BuildingAttackType::EAttackType::AT_TYPE_PROJECTILE) {
                m_entityManager->createProjectile(entity,
                                                  unit_component_utils::getTeam(entity),
                                                  base_component_utils::getPosition(entity),
                                                  base_component_utils::getPosition(targetEntity));
              }
              attackTimer.restart();
            } else {
              if(!AttackBaseSystem::trySearchForTarget(entity,
                                                       [](const anax::Entity&, const anax::Entity&) {})) {
                targetEntity = anax::Entity();
              }
            }
          }
        } else {
          if(searchTimer >= 1.f) {
            AttackBaseSystem::trySearchForTarget(entity,
                                                 [](const anax::Entity&, const anax::Entity&) {});
          }
        }
      }
    }

    if(searchTimer >= 1.f) {
      m_enemySearchTimer.restart();
    }
  }
}
}
