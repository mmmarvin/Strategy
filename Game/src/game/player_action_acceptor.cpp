#include "player_action_acceptor.h"

#include "../core/collider/collision_test.h"
#include "data/building_data.h"
#include "data/unit_data.h"
#include "entity/component/building_components.h"
#include "entitymanager.h"
#include "game_def.h"
#include "globalresourcemanager.h"

namespace strat
{
namespace action_acceptor
{
  GlobalResourceManager::EResourceQueueResult canBuild(const BuildingData& buildingData,
                                                       Player::EPlayerNumber team,
                                                       GlobalResourceManager& globalResourceManager)
  {
    auto buildResult = globalResourceManager.canBuild(buildingData, team);
    if(buildResult == GlobalResourceManager::RMR_SUCCESS) {
      globalResourceManager.withdrawTobuild(buildingData, team);
    }

    return buildResult;
  }

  bool canBuild(const BuildingData &buildingData,
                const gengine2d::Vector2f& position,
                EntityManager& entityManager)
  {
    auto tryCollider = gengine2d::Circlef(position.x - buildingData.colliderOffsetX,
                                          position.y - buildingData.colliderOffsetY,
                                          buildingData.colliderRadius);
    auto searchRadius = floorf((buildingData.colliderRadius / TILE_SIZE) + 2.f);
    auto sp = position / TILE_SIZE;
    auto findRes = entityManager.getSpatialPartition().findIf(sp.x - searchRadius,
                                                              sp.y - searchRadius,
                                                              sp.x + searchRadius,
                                                              sp.y + searchRadius,
    [&tryCollider](const anax::Entity& otherEntity) {
      if(base_component_utils::isOfType(otherEntity,
                                        component::BaseEntityType::EEntityType::Projectile) &&
         base_component_utils::isOfType(otherEntity,
                                        component::BaseEntityType::EEntityType::Flag)) {
        const auto& otherPosition = base_component_utils::getPosition(otherEntity);
        const auto& otherBaseCollider = base_component_utils::getBaseCollider(otherEntity);
        gengine2d::Circlef otherTryCollider(otherPosition.x - otherBaseCollider.center.x,
                                            otherPosition.y - otherBaseCollider.center.y,
                                            otherBaseCollider.radius);
        if(gengine2d::intersects(tryCollider, otherTryCollider)) {
          return true;
        }
      }

      return false;
    });

    return !findRes.first;
  }

  anax::Entity build(const BuildingData& buildingData,
                     const gengine2d::Vector2f& position,
                     Player::EPlayerNumber team,
                     EntityManager& entityManager)
  {
    return entityManager.createBuilding(buildingData, team, position, false);
  }

  GlobalResourceManager::EResourceQueueResult canTrain(const UnitData& unitData,
                                                       Player::EPlayerNumber team,
                                                       GlobalResourceManager& globalResourceManager)
  {
    auto trainResult = globalResourceManager.canTrain(unitData, team);
    if(trainResult == GlobalResourceManager::RMR_SUCCESS) {
      globalResourceManager.withdrawTotrain(unitData, team);
    }

    return trainResult;
  }

  void train(const UnitData& unitData,
             const anax::Entity& buildingEntity)
  {
    building_component_utils::addToUnitTrainQueue(buildingEntity,
                                                  std::make_pair(std::cref(unitData),
                                                                 mar::high_resolution_stopwatch()));
  }
}
}
