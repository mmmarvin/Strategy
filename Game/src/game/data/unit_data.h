#ifndef UNIT_DATA_H
#define UNIT_DATA_H

#include "attack_data.h"

namespace strat
{
  struct UnitData : public AttackData
  {
    enum EUnitType : unsigned int
    {
      UT_WORKER = 1 << 0,
      UT_SOLDIER = 1 << 1
    };

    gengine2d::string_t                       description;

    float                                     moveSpeed;
    float                                     armor;

    EUnitType                                 unitType;
    float                                     attackAnimationSpeed;

    gengine2d::vector_t<gengine2d::string_t>  buildingsBuilt;
  };
}

#endif // UNIT_DATA_H
