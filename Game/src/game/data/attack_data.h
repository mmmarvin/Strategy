#ifndef ATTACK_DATA_H
#define ATTACK_DATA_H

#include "base_data.h"

namespace strat
{
  struct AttackData : public BaseData
  {
    enum class EAttackType : unsigned int
    {
      AT_TYPE_NONE,
      AT_TYPE_MELEE,
      AT_TYPE_PROJECTILE
    };

    AttackData();

    EAttackType         attackType;
    float               attackDamage;
    float               attackDamageDropoff;
    float               attackDamageMin;
    float               attackDamageRadius;
    float               attackRange;
    float               attackSpeed;
  };
}

#endif // ATTACK_DATA_H
