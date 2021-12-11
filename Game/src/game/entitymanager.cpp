#include "entitymanager.h"

#include "../core/collider/collision_test.h"
#include "../core/utility/general_utility.h"
#include "../core/utility/vector_utility.h"
#include "entity/component/base_components.h"
#include "entity/component/building_components.h"
#include "entity/component/flag_components.h"
#include "entity/component/projectile_components.h"
#include "entity/component/resource_components.h"
#include "map/map.h"
#include "utility/position_utility.h"
#include "game_def.h"

namespace strat
{
namespace
{
  component::UnitSoldierAttackType::EAttackType convertUnitDataAttackType(AttackData::EAttackType type)
  {
    switch(type) {
    case AttackData::EAttackType::AT_TYPE_PROJECTILE:
      return component::UnitSoldierAttackType::EAttackType::AT_TYPE_PROJECTILE;
    case AttackData::EAttackType::AT_TYPE_MELEE:
    case AttackData::EAttackType::AT_TYPE_NONE:
    default:
      break;
    }

    return component::UnitSoldierAttackType::EAttackType::AT_TYPE_MELEE;
  }

  component::BuildingAttackType::EAttackType convertBuildingDataAttackType(AttackData::EAttackType type)
  {
    switch(type) {
    case AttackData::EAttackType::AT_TYPE_PROJECTILE:
      return component::BuildingAttackType::EAttackType::AT_TYPE_PROJECTILE;
    case AttackData::EAttackType::AT_TYPE_MELEE:
      return component::BuildingAttackType::EAttackType::AT_TYPE_MELEE;
    case AttackData::EAttackType::AT_TYPE_NONE:
    default:
      break;
    }

    return component::BuildingAttackType::EAttackType::AT_TYPE_NONE;
  }
}
  EntityManager::EntityManager(const Map& map,
                               FogManager& fogs) :
    m_map(&map),
    m_fogs(&fogs)
  {
  }

  anax::Entity EntityManager::createUnit(const UnitData& unitData,
                                         Player::EPlayerNumber team,
                                         gengine2d::Vector2f position,
                                         const anax::Entity& castle)
  {
    auto entity = createBaseEntity(unitData,
                                   position,
                                   component::BaseEntityType::EEntityType::Unit,
                                   true);

    component::UnitType::EUnitType type = static_cast<component::UnitType::EUnitType>(0);
    if(unitData.unitType & UnitData::UT_WORKER) {
      type = static_cast<component::UnitType::EUnitType>(type | component::UnitType::Worker);
    }
    if(unitData.unitType & UnitData::UT_SOLDIER) {
      type = static_cast<component::UnitType::EUnitType>(type | component::UnitType::Soldier);
    }
    assert(type != 0);
    addUnitComponents(unitData, entity, team, type);

    if(type & component::UnitType::EUnitType::Soldier) {
      component::UnitSoldierAttackType::EAttackType attackType = convertUnitDataAttackType(unitData.attackType);

      entity.addComponent<component::UnitSoldierAttackType>(attackType);
      entity.addComponent<component::UnitSoldierAttackAttributes>(std::initializer_list<float>{
        unitData.attackDamage,            // Attack Damage
        unitData.attackRange * UNIT_SIZE, // Attack Range
        unitData.attackSpeed,             // Attack Speed
        unitData.attackAnimationSpeed     // Attack Animation Speed
      });
      entity.addComponent<component::UnitSoldierAttackTimer>();
      entity.addComponent<component::UnitSoldierWaitTimer>();
    }

    if(type & component::UnitType::EUnitType::Worker) {
      assert(castle.isValid());

      entity.addComponent<component::UnitWorkerGatherTimer>();
      entity.addComponent<component::UnitWorkerResourceCollected>();
      entity.addComponent<component::UnitWorkerResourceGathering>();
      entity.addComponent<component::UnitWorkerCastle>(anax::Entity());
      entity.addComponent<component::UnitWorkerBuildingsBuilts>(unitData.buildingsBuilt);
    }

    m_entityWorld.activateEntity(entity);
//    m_entityWorld.refresh();

    const auto& spatialPosition = base_component_utils::getSpatialPositionHandle(entity);
    m_fogs->getFog(team).exposeRadius(spatialPosition.getPosition().x,
                                      spatialPosition.getPosition().y,
                                      1.f);

    return entity;
  }

  anax::Entity EntityManager::createBuilding(const BuildingData& buildingData,
                                             Player::EPlayerNumber team,
                                             gengine2d::Vector2f position,
                                             bool completed)
  {
    auto entity = createBaseEntity(buildingData,
                                   position,
                                   component::BaseEntityType::EEntityType::Building);
    auto& battribs = entity.getComponent<component::BaseAttributes>().attributes;
    battribs[component::BaseAttributes::BA_ATTRIB_HP] = 1.f;

    entity.addComponent<component::UnitTeam>(team);
    entity.addComponent<component::BuildingCompleteness>(completed);
    entity.addComponent<component::BuildingType>(static_cast<component::BuildingType::EBuildingType>(static_cast<unsigned int>(buildingData.type)));
    if(completed) {
      battribs[component::BaseAttributes::BA_ATTRIB_HP] = battribs[component::BaseAttributes::BA_ATTRIB_MAX_HP];
    }

    // Attack Components
    entity.addComponent<component::UnitTarget>();
    entity.addComponent<component::BuildingAttackType>(convertBuildingDataAttackType(buildingData.attackType));
    entity.addComponent<component::BuildingAttackAttributes>(std::initializer_list<float>{
      buildingData.attackDamage,            // Attack Damage
      buildingData.attackRange * UNIT_SIZE, // Attack Range
      buildingData.attackSpeed
    });
    entity.addComponent<component::BuildingAttackTimer>();

    // Unit Trainer Components
    entity.addComponent<component::BuildingUnitsTrained>(buildingData.unitsTrained);
    entity.addComponent<component::BuildingUnitTrainQueue>();
    const auto& buildingUnitTrainTargetC = entity.addComponent<component::BuildingUnitTrainTarget>();
    auto flagEntity = createFlag(entity,
                                 team,
                                 buildingUnitTrainTargetC.targetPosition);
    entity.addComponent<component::BuildingUnitTrainFlag>(flagEntity);

    m_entityWorld.activateEntity(entity);

    const auto& spatialPosition = base_component_utils::getSpatialPositionHandle(entity);
    m_fogs->getFog(team).exposeRadius(spatialPosition.getPosition().x,
                                      spatialPosition.getPosition().y,
                                      2.f);

    return entity;
  }

  anax::Entity EntityManager::createResource(gengine2d::Vector2f position,
                                             GlobalResource::EGlobalResouceType type)
  {
    BaseData baseData;
    baseData.name = "Base";
    baseData.colliderOffsetX = 0.f;
    baseData.colliderOffsetY = 0.f;
    baseData.colliderRadius = 1.f;
    baseData.health = 100.f;

    auto entity = createBaseEntity(baseData,
                                   position,
                                   component::BaseEntityType::EEntityType::Resource);

    entity.addComponent<component::ResourceType>(type);
    entity.addComponent<component::ResourceAttributes>(std::initializer_list<int>{
      100,    // Resource left
      100     // Resource max
    });

    m_entityWorld.activateEntity(entity);

    return entity;
  }

  anax::Entity EntityManager::createProjectile(const anax::Entity& owner,
                                               Player::EPlayerNumber team,
                                               gengine2d::Vector2f position,
                                               const gengine2d::Vector2f& targetPosition)
  {
    BaseData baseData;
    baseData.name = "Base";
    baseData.colliderOffsetX = 0.f;
    baseData.colliderOffsetY = 0.f;
    baseData.colliderRadius = 1.f;
    baseData.health = 100.f;

    auto entity = createBaseEntity(baseData,
                                   position,
                                   component::BaseEntityType::EEntityType::Projectile);
    entity.addComponent<component::UnitTeam>(team);
    entity.addComponent<component::ProjectileOwner>(owner);
    entity.addComponent<component::ProjectileTargetPosition>(targetPosition);
    entity.addComponent<component::ProjectileAttributes>(std::initializer_list<float>{
      50.f * UNIT_SIZE, // Move speed
      10.f,             // Attack damage,
      5.f,              // Attack damage dropoff
      5.f,              // Attack damage min
      40.f * UNIT_SIZE  // Damage radius
    });

    // remove unused components
    entity.removeComponent<component::BaseCollider>();
    entity.removeComponent<component::BaseAttributes>();
    entity.removeComponent<component::BaseFlags>();

    m_entityWorld.activateEntity(entity);

    return entity;
  }

  anax::Entity EntityManager::createHomingProjectile(const anax::Entity& owner,
                                                     Player::EPlayerNumber team,
                                                     gengine2d::Vector2f position,
                                                     const anax::Entity& targetEntity)
  {
    BaseData baseData;
    baseData.name = "Base";
    baseData.colliderOffsetX = 0.f;
    baseData.colliderOffsetY = 0.f;
    baseData.colliderRadius = 1.f;
    baseData.health = 100.f;

    auto entity = createBaseEntity(baseData,
                                   position,
                                   component::BaseEntityType::EEntityType::Projectile);

    entity.addComponent<component::UnitTeam>(team);
    entity.addComponent<component::ProjectileOwner>(owner);
    entity.addComponent<component::ProjectileTargetEntity>(targetEntity);
    entity.addComponent<component::ProjectileAttributes>(std::initializer_list<float>{
      50.f * UNIT_SIZE,  // Move speed
      10.f,              // Attack damage,
      5.f,               // Attack damage dropoff
      5.f,               // Attack damage min
      40.f * UNIT_SIZE   // Damage radius
    });

    // remove unused components
    entity.removeComponent<component::BaseCollider>();
    entity.removeComponent<component::BaseAttributes>();
    entity.removeComponent<component::BaseFlags>();

    m_entityWorld.activateEntity(entity);

    return entity;
  }

  anax::Entity EntityManager::createFlag(const anax::Entity& owner,
                                         Player::EPlayerNumber team,
                                         gengine2d::Vector2f position)
  {
    auto entity = createFlagBase(position);
    entity.addComponent<component::UnitTeam>(team);
    entity.addComponent<component::FlagOwner>(owner);

    m_entityWorld.activateEntity(entity);

    return entity;
  }

  anax::Entity EntityManager::createFlagTimed(gengine2d::Vector2f position)
  {
    auto entity = createFlagBase(position);
    entity.addComponent<component::FlagTimer>();

    m_entityWorld.activateEntity(entity);

    return entity;
  }

  void EntityManager::scheduleForDeletion(const anax::Entity& entity)
  {
    auto entityId = entity.getId().value();
    if(m_entitiesDeletedId.find(entityId) == m_entitiesDeletedId.end()) {
      if(entity.hasComponent<component::BuildingUnitTrainFlag>()) {
        // if entity has a train flag, we must also schedule the flag entity for deletion
        m_entitiesDeleted.push_back(building_component_utils::getUnitTrainFlagEntity(entity));
      }

      m_entitiesDeleted.push_back(entity);
      m_entitiesDeletedId.emplace(entityId);
    }
  }

  anax::Entity EntityManager::createBaseEntity(const BaseData& baseData,
                                               gengine2d::Vector2f& position,
                                               component::BaseEntityType::EEntityType type,
                                               bool moveToOpenSpot)
  {
    auto entity = createBaseNonSpatialEntity(baseData, position, type, moveToOpenSpot);
    entity.addComponent<component::BaseSpatialPosition>(m_spatialPartition.add(position.x / TILE_SIZE,
                                                                               position.y / TILE_SIZE,
                                                                               entity));

    return entity;
  }

  anax::Entity EntityManager::createBaseNonSpatialEntity(const BaseData& baseData,
                                                         gengine2d::Vector2f& position,
                                                         component::BaseEntityType::EEntityType type,
                                                         bool moveToOpenSpot)
  {
    if(moveToOpenSpot) {
      auto trySpawn = [&position, this](const gengine2d::Vector2f& colliderPosition,
                                        float colliderOffsetX,
                                        float colliderOffsetY,
                                        float colliderRadius) {
        auto spx = colliderPosition.x / TILE_SIZE;
        auto spy = colliderPosition.y / TILE_SIZE;
        auto entityColliding = m_spatialPartition.findIf(spx - 2.f,
                                                         spy - 2.f,
                                                         spx + 2.f,
                                                         spy + 2.f,
        [colliderRadius, &colliderPosition](const anax::Entity& otherEntity) {
          if(!base_component_utils::isOfType(otherEntity,
                                             component::BaseEntityType::EEntityType::Projectile) &&
             !base_component_utils::isOfType(otherEntity,
                                             component::BaseEntityType::EEntityType::Flag)) {
            const auto& otherBaseCollider = base_component_utils::getBaseCollider(otherEntity);
            const auto& otherPosition = base_component_utils::getPosition(otherEntity);
            auto otherColliderPosition = otherPosition - otherBaseCollider.center;

            auto d = gengine2d::distance(colliderPosition, otherColliderPosition);
            if(d <= (colliderRadius + otherBaseCollider.radius + UNIT_SIZE)) {
              return true;
            }
          }

          return false;
        });

        if(!entityColliding.first) {
          position = gengine2d::Vector2f(colliderPosition.x + colliderOffsetX,
                                         colliderPosition.y + colliderOffsetY);
          return true;
        }

        return false;
      };

      auto tryPosition = position;
      bool done = false;
      while(!done) {
        // TODO: Change this if change separate steering
        auto colliderOffsetX = baseData.colliderOffsetX;
        auto colliderOffsetY = baseData.colliderOffsetY;
        auto colliderRadius = baseData.colliderRadius * UNIT_SIZE;
        auto colliderPosition = tryPosition - gengine2d::Vector2f(colliderOffsetX, colliderOffsetY);

        if(trySpawn(colliderPosition, colliderOffsetX, colliderOffsetY, colliderRadius)) {
          break;
        }
        for(float mult = 2.f; mult <= 4.f; mult += 2.f) {
          if(trySpawn(colliderPosition - gengine2d::Vector2f((colliderRadius * mult) + 1.f, 0.f), colliderOffsetX, colliderOffsetY, colliderRadius)) {
            done = true;
            break;
          }
          if(trySpawn(colliderPosition + gengine2d::Vector2f((colliderRadius * mult) + 1.f, 0.f), colliderOffsetX, colliderOffsetY, colliderRadius)) {
            done = true;
            break;
          }
        }
        tryPosition.y += ((colliderRadius * 2.f) + 1.f);
      }
    }

    auto baseCollider = gengine2d::Circlef(baseData.colliderOffsetX,
                                           baseData.colliderOffsetY,
                                           baseData.colliderRadius * UNIT_SIZE);
    if(type != component::BaseEntityType::EEntityType::Flag) {
      position = position_utils::clampEntityPosition(position,
                                                     baseCollider,
                                                     gengine2d::vector_utils::convertTo<float>(m_map->getSize()));
    }

    auto entity = m_entityWorld.createEntity();
    entity.addComponent<component::BaseName>(baseData.name);
    entity.addComponent<component::BasePosition>(position);
    entity.addComponent<component::BaseSprite>();
    entity.addComponent<component::BaseCollider>(baseCollider);
    entity.addComponent<component::BaseEntityType>(type);

    entity.addComponent<component::BaseAttributes>(std::initializer_list<float>{
      baseData.health,    // Current HP
      baseData.health     // Max HP
    });
    entity.addComponent<component::BaseFlags>(std::initializer_list<bool>{
      false,    // Selected
      true,     // Collideable
      false     // Invulnerable
    });

    return entity;
  }

  anax::Entity EntityManager::createFlagBase(gengine2d::Vector2f& position)
  {
    auto entity = createBaseEntity(BaseData(),
                                   position,
                                   component::BaseEntityType::EEntityType::Flag);
    entity.removeComponent<component::BaseCollider>();
    entity.removeComponent<component::BaseAttributes>();
    entity.removeComponent<component::BaseFlags>();
    return entity;
  }

  void EntityManager::addUnitComponents(const UnitData& unitData,
                                        anax::Entity& entity,
                                        Player::EPlayerNumber team,
                                        component::UnitType::EUnitType type)
  {
    entity.addComponent<component::UnitAttributes>(std::initializer_list<float>{
      unitData.moveSpeed * UNIT_SIZE, // Movespeed
      unitData.armor                  // Armor
    });
    entity.addComponent<component::UnitTeam>(team);
    entity.addComponent<component::UnitState>();
    entity.addComponent<component::UnitTarget>();
    entity.addComponent<component::UnitType>(type);
  }

  void EntityManager::refresh()
  {
    for(auto& entity : m_entitiesDeleted) {
      if(entity.hasComponent<component::BaseSpatialPosition>()) {
        m_spatialPartition.remove(entity.getComponent<component::BaseSpatialPosition>().spatialPosition);
      }

      m_entityWorld.killEntity(entity);
    }

    m_entitiesDeleted.clear();
    m_entitiesDeletedId.clear();

    m_entityWorld.refresh();
  }
}
