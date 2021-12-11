#ifndef PLAYER_ACTION_ACCEPTOR_H
#define PLAYER_ACTION_ACCEPTOR_H

#include <anax/anax.hpp>
#include "../core/vector2.h"
#include "globalresourcemanager.h"
#include "player.h"

namespace strat
{
  class BuildingData;
  class UnitData;
  class EntityManager;
namespace action_acceptor
{
  GlobalResourceManager::EResourceQueueResult canBuild(const BuildingData& buildingData,
                                                       Player::EPlayerNumber team,
                                                       GlobalResourceManager& globalResourceManager);
  bool canBuild(const BuildingData &buildingData,
                const gengine2d::Vector2f& position,
                EntityManager& entityManager);
  anax::Entity build(const BuildingData& buildingData,
                     const gengine2d::Vector2f& position,
                     Player::EPlayerNumber team,
                     EntityManager& entityManager);

  GlobalResourceManager::EResourceQueueResult canTrain(const UnitData& unitData,
                                                       Player::EPlayerNumber team,
                                                       GlobalResourceManager& globalResourceManager);
  void train(const UnitData& unitData,
             const anax::Entity& buildingEntity);
}
}

#endif // PLAYER_ACTION_ACCEPTOR_H
