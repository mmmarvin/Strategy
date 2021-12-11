#ifndef GLOBAL_RESOURCE_H
#define GLOBAL_RESOURCE_H

#include "../core/type.h"
#include "player.h"

namespace strat
{
  class GlobalResource
  {
  public:
    enum EGlobalResouceType : unsigned int
    {
      GR_RES_FOOD = 0,
      GR_RES_GOLD,
      GR_RES_STONE,
      GR_RES_WOOD,
      GR_NUMBER_OF_RES
    };

    GlobalResource();

    void addResource(Player::EPlayerNumber playerId, EGlobalResouceType resourceId, int value);
    bool removeResource(Player::EPlayerNumber playerId, EGlobalResouceType resourceId, int value);
    int getResourceCount(Player::EPlayerNumber playerId, EGlobalResouceType resourceId) const;

  private:
    int& get(Player::EPlayerNumber playerId, EGlobalResouceType resourceId);
    const int& get(Player::EPlayerNumber playerId, EGlobalResouceType resourceId) const;

    using resource_container_type = gengine2d::fixed_vector_t<int, GR_NUMBER_OF_RES>;
    using global_resource_container_type =
      gengine2d::fixed_vector_t<resource_container_type, Player::EPlayerNumber::PN_MAX_PLAYER_NUMBER>;

    global_resource_container_type m_globalResource;
  };
namespace string_converter_utils
{
  gengine2d::string_t to_string(GlobalResource::EGlobalResouceType type);
}
}

#endif // GLOBAL_RESOURCE_H
