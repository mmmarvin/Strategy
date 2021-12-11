#ifndef BASE_DATA_H
#define BASE_DATA_H

#include "../../core/type.h"
#include "../global_resource.h"

namespace strat
{
  struct ResourceRequirement
  {
    ResourceRequirement();

    bool hasRequriements() const;
    bool hasGoldRequirements() const;

    gengine2d::fixed_vector_t<int, GlobalResource::GR_NUMBER_OF_RES> requirements;
    float                                                            creationTime;
  };

  struct BaseData
  {
    BaseData();

    gengine2d::string_t name;

    float               colliderOffsetX;
    float               colliderOffsetY;
    float               colliderRadius;
    float               health;

    ResourceRequirement creationRequirements;
  };


}

#endif // BASE_DATA_H
