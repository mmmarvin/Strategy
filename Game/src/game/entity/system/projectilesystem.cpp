#include "projectilesystem.h"

#include "../../../core/collider/collision_test.h"
#include "../../../core/game.h"
#include "../../utility/steering_utility.h"
#include "../../entitymanager.h"
#include "../../game_def.h"

namespace strat
{
namespace system
{
namespace
{
  void damageSurroundings(AttackBaseSystem& attackBaseSystem,
                          spatial_partition_t& spatialPartition,
                          const gengine2d::Circlef& blastCollider,
                          float minX,
                          float minY,
                          float maxX,
                          float maxY,
                          float damage,
                          float damageDropoff,
                          float damageMin,
                          const anax::Entity& entity,
                          const gengine2d::Vector2f& position)
  {
    spatialPartition.forEach(minX,
                             minY,
                             maxX,
                             maxY,
    [&attackBaseSystem, &blastCollider, &entity, &position, damage, damageDropoff, damageMin]
    (const anax::Entity& otherEntity) {
      if(base_component_utils::isOfType(otherEntity,
                                        component::BaseEntityType::EEntityType::Unit) ||
         base_component_utils::isOfType(otherEntity,
                                        component::BaseEntityType::EEntityType::Building)) {
        if(!unit_component_utils::sameTeam(entity, otherEntity)) {
          const auto& otherPosition = otherEntity.getComponent<component::BasePosition>().position;
          auto d = gengine2d::distance(position, otherPosition);
          if(d <= UNIT_SIZE) {
            attackBaseSystem.performAttack(entity, otherEntity, damage);
          } else {
            if(gengine2d::contains(blastCollider, otherPosition)) {
              attackBaseSystem.performAttack(entity,
                                             otherEntity,
                                             gengine2d::general_utils::max(damage - (damageDropoff * (d / 100.f)),
                                                      damageMin));
            }
          }
        }
      }
    });
  }
}
  // TODO: Should projectiles cause exposure of map in fog of war?
  /// Projectile System
  ProjectileSystem::ProjectileSystem(EntityManager& entityManager,
                                     const Map& map,
                                     FogManager& fogs) :
    AttackBaseSystem(entityManager, map, fogs)
  {
  }

  void ProjectileSystem::draw() const
  {
    // FIXME: Draw Projectile Blast Radius
//    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
//    for(auto& [blastShape, blastDurationTimer] : m_blastRadiuses) {
//      renderer.draw(blastShape);
//    }
  }

  void ProjectileSystem::update(float dt)
  {
    for(const auto& entity : this->getEntities()) {
      auto& positionC = entity.getComponent<component::BasePosition>();
      auto& position = positionC.position;
      auto& oldPosition = positionC.oldPosition;
      const auto& targetPosition = entity.getComponent<component::ProjectileTargetPosition>().targetPosition;
      const auto& projAttribs = entity.getComponent<component::ProjectileAttributes>().attributes;

      if(gengine2d::distance(position, targetPosition) <= UNIT_SIZE) {
        auto blastRadius = projAttribs[component::ProjectileAttributes::PA_ATTRIB_DAMAGE_RADIUS];
        auto searchRadiusX = blastRadius / TILE_SIZE;
        auto searchRadiusY = blastRadius / TILE_SIZE;

        // TODO: Use spatial position instead?
        auto spx = position.x / TILE_SIZE;
        auto spy = position.y / TILE_SIZE;

        auto blastDamage = projAttribs[component::ProjectileAttributes::PA_ATTRIB_ATTACK_DAMAGE];
        auto blastDamageDropOff = projAttribs[component::ProjectileAttributes::PA_ATTRIB_ATTACK_DAMAGE_DROPOFF];
        auto blastDamageMin = projAttribs[component::ProjectileAttributes::PA_ATTRIB_ATTACK_DAMAGE_MIN];

//        circle_shape_type blastShape(blastRadius);
//        blastShape.setFillColor(Color(255, 0, 0, 32));
//        blastShape.setOrigin(blastRadius, blastRadius);
//        blastShape.setPosition(gengine2d::vector_utils::convertCartToIsoPoint(position));
//        m_blastRadiuses.push_back(std::make_pair(blastShape, mar::high_resolution_clock()));

        damageSurroundings(*this,
                           m_entityManager->getSpatialPartition(),
                           gengine2d::Circlef(position.x, position.y, blastRadius),
                           spx - searchRadiusX - 1.f,
                           spy - searchRadiusY - 1.f,
                           spx + searchRadiusX + 1.f,
                           spy + searchRadiusY + 1.f,
                           blastDamage,
                           blastDamageDropOff,
                           blastDamageMin,
                           entity,
                           position);

        // TODO: Schedule for deletion
        m_entityManager->scheduleForDeletion(entity);
      } else {
        auto moveSpeed = projAttribs[component::ProjectileAttributes::PA_ATTRIB_MOVE_SPEED];

        oldPosition = position;
        auto adjustedMoveSpeed = moveSpeed * dt;

        gengine2d::Vector2f vel;
        auto seekForce = steering_utils::seek(position,
                                              targetPosition,
                                              vel,
                                              adjustedMoveSpeed);
        steering_utils::accumulateForce(vel, seekForce, adjustedMoveSpeed);

        position += vel;
        spatialpartition_utils::updateSpatialPosition(entity,
                                                      position,
                                                      base_component_utils::getSpatialPositionHandle(entity),
                                                      m_entityManager->getSpatialPartition());
      }
    }

//    for(std::size_t i = 0, isize = m_blastRadiuses.size(); i < isize; ++i) {
//      auto& [blastShape, blastDurationTimer] = m_blastRadiuses[i];
//      if(blastDurationTimer.get_elapsed().as_seconds() >= 3.f) {
//        std::swap(m_blastRadiuses[i], m_blastRadiuses.back());
//        m_blastRadiuses.pop_back();
//        isize = m_blastRadiuses.size();
//      } else {
//        ++i;
//      }
//    }
  }

  /// Projectile Homing System
  ProjectileHomingSystem::ProjectileHomingSystem(EntityManager& entityManager,
                                                 const Map& map,
                                                 FogManager& fogs) :
    AttackBaseSystem(entityManager, map, fogs)
  {
  }

  void ProjectileHomingSystem::draw() const
  {
    // FIXME: Draw blast radius
//    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
//    for(auto& [blastShape, blastDurationTimer] : m_blastRadiuses) {
//      renderer.draw(blastShape);
//    }
  }

  void ProjectileHomingSystem::update(float dt)
  {
    for(const auto& entity : this->getEntities()) {
      auto& positionC = entity.getComponent<component::BasePosition>();
      auto& position = positionC.position;
      auto& oldPosition = positionC.oldPosition;
      auto& targetEntityC = entity.getComponent<component::ProjectileTargetEntity>();
      const auto& projAttribs = entity.getComponent<component::ProjectileAttributes>().attributes;

      auto& targetEntity = targetEntityC.targetEntity;
      auto& targetPosition = targetEntityC.targetPosition;
      if(base_component_utils::isValidOrReset(targetEntity)) {
        targetPosition = targetEntity.getComponent<component::BasePosition>().position;

        if(gengine2d::distance(position, targetPosition) <= UNIT_SIZE) {
          this->performAttack(entity,
                              targetEntity,
                              projAttribs[component::ProjectileAttributes::PA_ATTRIB_ATTACK_DAMAGE]);

          auto blastRadius = projAttribs[component::ProjectileAttributes::PA_ATTRIB_DAMAGE_RADIUS];
          if(blastRadius > 0.f) {
            auto searchRadiusX = blastRadius / TILE_SIZE;
            auto searchRadiusY = blastRadius / TILE_SIZE;

            // TODO: Use spatial position instead?
            auto spx = position.x / TILE_SIZE;
            auto spy = position.y / TILE_SIZE;

            auto blastDamage = projAttribs[component::ProjectileAttributes::PA_ATTRIB_ATTACK_DAMAGE];
            auto blastDamageDropOff = projAttribs[component::ProjectileAttributes::PA_ATTRIB_ATTACK_DAMAGE_DROPOFF];
            auto blastDamageMin = projAttribs[component::ProjectileAttributes::PA_ATTRIB_ATTACK_DAMAGE_MIN];

//            circle_shape_type blastShape(blastRadius);
//            blastShape.setFillColor(Color(255, 0, 0, 32));
//            blastShape.setOrigin(blastRadius, blastRadius);
//            blastShape.setPosition(gengine2d::vector_utils::convertCartToIsoPoint(position));
//            m_blastRadiuses.push_back(std::make_pair(blastShape, mar::high_resolution_clock()));

            damageSurroundings(*this,
                               m_entityManager->getSpatialPartition(),
                               gengine2d::Circlef(position.x, position.y, blastRadius),
                               spx - searchRadiusX - 1.f,
                               spy - searchRadiusY - 1.f,
                               spx + searchRadiusX + 1.f,
                               spy + searchRadiusY + 1.f,
                               blastDamage,
                               blastDamageDropOff,
                               blastDamageMin,
                               entity,
                               position);
          }

          // TODO: Schedule for deletion
          m_entityManager->scheduleForDeletion(entity);
        } else {
          oldPosition = position;

          auto moveSpeed = projAttribs[component::ProjectileAttributes::PA_ATTRIB_MOVE_SPEED];

          oldPosition = position;
          auto adjustedMoveSpeed = moveSpeed * dt;

          gengine2d::Vector2f vel;
          auto seekForce = steering_utils::seek(position,
                                                targetPosition,
                                                vel,
                                                adjustedMoveSpeed);
          steering_utils::accumulateForce(vel, seekForce, adjustedMoveSpeed);

          position += vel;
          spatialpartition_utils::updateSpatialPosition(entity,
                                                        position,
                                                        base_component_utils::getSpatialPositionHandle(entity),
                                                        m_entityManager->getSpatialPartition());
        }
      } else {
        // TODO: Schedule for deletion
        m_entityManager->scheduleForDeletion(entity);
      }
    }

//    for(std::size_t i = 0, isize = m_blastRadiuses.size(); i < isize; ++i) {
//      auto& [blastShape, blastDurationTimer] = m_blastRadiuses[i];
//      if(blastDurationTimer.get_elapsed().as_seconds() >= 3.f) {
//        std::swap(m_blastRadiuses[i], m_blastRadiuses.back());
//        m_blastRadiuses.pop_back();
//        isize = m_blastRadiuses.size();
//      } else {
//        ++i;
//      }
//    }
  }
}
}
