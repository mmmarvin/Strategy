#include "globalresourcemanager.h"

#include "data/building_data.h"
#include "data/unit_data.h"

namespace strat
{
  GlobalResourceManager::EResourceQueueResult GlobalResourceManager::canBuild(const BuildingData& data, Player::EPlayerNumber team) const
  {
    for(unsigned int i = 0; i < GlobalResource::GR_NUMBER_OF_RES; ++i) {
      if(m_globalResource.getResourceCount(team, static_cast<GlobalResource::EGlobalResouceType>(i)) <
         data.creationRequirements.requirements[i]) {
        return static_cast<EResourceQueueResult>(i + 1);
      }
    }

    return RMR_SUCCESS;
  }

  GlobalResourceManager::EResourceQueueResult GlobalResourceManager::canTrain(const UnitData& data, Player::EPlayerNumber team) const
  {
    for(unsigned int i = 0; i < GlobalResource::GR_NUMBER_OF_RES; ++i) {
      if(m_globalResource.getResourceCount(team, static_cast<GlobalResource::EGlobalResouceType>(i)) <
         data.creationRequirements.requirements[i]) {
        return static_cast<EResourceQueueResult>(i + 1);
      }
    }

    return RMR_SUCCESS;
  }

  void GlobalResourceManager::withdrawTobuild(const BuildingData& data, Player::EPlayerNumber team)
  {
    for(unsigned int i = 0; i < GlobalResource::GR_NUMBER_OF_RES; ++i) {
      m_globalResource.addResource(team,
                                   static_cast<GlobalResource::EGlobalResouceType>(i),
                                   -data.creationRequirements.requirements[i]);
    }
  }

  void GlobalResourceManager::withdrawTotrain(const UnitData& data, Player::EPlayerNumber team)
  {
    for(unsigned int i = 0; i < GlobalResource::GR_NUMBER_OF_RES; ++i) {
      m_globalResource.addResource(team,
                                   static_cast<GlobalResource::EGlobalResouceType>(i),
                                   -data.creationRequirements.requirements[i]);
    }
  }
}
