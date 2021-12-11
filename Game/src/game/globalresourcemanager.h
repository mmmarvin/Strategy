#ifndef GLOBALRESOURCEMANAGER_H
#define GLOBALRESOURCEMANAGER_H

#include "global_resource.h"

namespace strat
{
  class UnitData;
  class BuildingData;
  class GlobalResourceManager
  {
  public:
    enum EResourceQueueResult
    {
      RMR_SUCCESS = 0,
      RMR_NO_FOOD,
      RMR_NO_GOLD,
      RMR_NO_STONE,
      RMR_NO_WOOD
    };

    GlobalResourceManager() {}

    EResourceQueueResult canBuild(const BuildingData& data, Player::EPlayerNumber team) const;
    EResourceQueueResult canTrain(const UnitData& data, Player::EPlayerNumber team) const;
    void withdrawTobuild(const BuildingData& data, Player::EPlayerNumber team);
    void withdrawTotrain(const UnitData& data, Player::EPlayerNumber team);

    GlobalResource& getGlobalResource() { return m_globalResource; }
    const GlobalResource& getGlobalResource() const { return m_globalResource; }

  private:
    GlobalResource  m_globalResource;
  };
}

#endif // GLOBALRESOURCEMANAGER_H
