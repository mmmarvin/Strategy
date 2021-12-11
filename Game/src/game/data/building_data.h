#ifndef BUILDING_DATA_H
#define BUILDING_DATA_H

#include "attack_data.h"

namespace strat
{
  struct BuildingData : public AttackData
  {
    enum class EBuildingType : unsigned char
    {
      BT_TYPE_CASTLE = 0,
      BT_TYPE_TRAINER,
      BT_TYPE_TOWER,
      BT_TYPE_WALL,
      BT_TYPE_OTHER
    };

    gengine2d::string_t                       description;
    gengine2d::vector_t<gengine2d::string_t>  unitsTrained;
    EBuildingType                             type;
  };
}

#endif // BUILDING_DATA_H
