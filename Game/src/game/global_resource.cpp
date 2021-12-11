#include "global_resource.h"

#include <imgui.h>
#include "../core/debug/debug_log.h"
#include "../core/utility/stl_utility.h"
#include "../core/game.h"
#include "game_def.h"

namespace strat
{
  GlobalResource::GlobalResource()
  {
    // Set everything to 0
    for(unsigned int i = Player::EPlayerNumber::PN_PLAYER1;
        i < Player::EPlayerNumber::PN_MAX_PLAYER_NUMBER;
        ++i) {
      auto& playerResource = m_globalResource[i];
      for(unsigned int j = EGlobalResouceType::GR_RES_FOOD;
          j < EGlobalResouceType::GR_NUMBER_OF_RES;
          ++j) {
        playerResource[j] = 0;
      }
    }
  }

  void GlobalResource::addResource(Player::EPlayerNumber playerId,
                                   EGlobalResouceType resourceId,
                                   int value)
  {
    long int resourceCount = getResourceCount(playerId, resourceId);
    if(resourceCount + value < RESOURCE_MAX_VALUE) {
      get(playerId, resourceId) += value;
    } else {
      get(playerId, resourceId) = RESOURCE_MAX_VALUE;
    }
  }

  bool GlobalResource::removeResource(Player::EPlayerNumber playerId,
                                      EGlobalResouceType resourceId,
                                      int value)
  {
    auto& resourceCount = get(playerId, resourceId);
    if(resourceCount >= value) {
      resourceCount -= value;
      return true;
    }

    return false;
  }

  int GlobalResource::getResourceCount(Player::EPlayerNumber playerId,
                                         EGlobalResouceType resourceId) const
  {
    return get(playerId, resourceId);
  }

  int& GlobalResource::get(Player::EPlayerNumber playerId, EGlobalResouceType resourceId)
  {
    return m_globalResource[playerId][resourceId];
  }

  const int& GlobalResource::get(Player::EPlayerNumber playerId, EGlobalResouceType resourceId) const
  {
    return m_globalResource[playerId][resourceId];
  }
namespace string_converter_utils
{
  gengine2d::string_t to_string(GlobalResource::EGlobalResouceType type)
  {
    switch(type) {
    case GlobalResource::EGlobalResouceType::GR_RES_FOOD:
      return "Food";
    case GlobalResource::EGlobalResouceType::GR_RES_GOLD:
      return "Gold";
    case GlobalResource::EGlobalResouceType::GR_RES_STONE:
      return "Stone";
    case GlobalResource::EGlobalResouceType::GR_RES_WOOD:
      return "Wood";
    default:
      break;
    }

    return "";
  }
}
}
