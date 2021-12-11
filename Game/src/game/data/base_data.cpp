#include "base_data.h"

namespace strat
{
  ResourceRequirement::ResourceRequirement() :
    requirements(0)
  {
  }

  bool ResourceRequirement::hasRequriements() const
  {
    for(unsigned int i = 0; i < GlobalResource::GR_NUMBER_OF_RES; ++i) {
      if(i != GlobalResource::GR_RES_GOLD) {
        if(requirements[i] > 0) {
          return true;
        }
      }
    }

    return false;
  }

  bool ResourceRequirement::hasGoldRequirements() const
  {
    return requirements[GlobalResource::GR_RES_GOLD] > 0;
  }

  BaseData::BaseData() :
    name(""),
    colliderOffsetX(0.f),
    colliderOffsetY(0.f),
    colliderRadius(0.f),
    health(0.f)
  {
  }
}
