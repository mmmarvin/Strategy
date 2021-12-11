#ifndef STEERING_UTILITY_H
#define STEERING_UTILITY_H

#include <anax/anax.hpp>
#include "../../core/vector2.h"
#include "../spatialpartition.h"

namespace strat
{
namespace steering_utils
{
  bool accumulateForce(gengine2d::Vector2f& total,
                       const gengine2d::Vector2f& force,
                       float moveSpeed);

  gengine2d::Vector2f seek(const gengine2d::Vector2f& position,
                           const gengine2d::Vector2f& target,
                           const gengine2d::Vector2f& currentVelocity,
                           float moveSpeed);

  gengine2d::Vector2f separate(const anax::Entity& entity,
                               const gengine2d::Vector2f& position,
                               gengine2d::Vector2f& currentVelocity,
                               float moveSpeed,
                               const spatial_partition_t& spatialPartition);

  gengine2d::Vector2f flee(const gengine2d::Vector2f& position,
                           const gengine2d::Vector2f& target,
                           const gengine2d::Vector2f& currentVelocity,
                           float moveSpeed);
}
}

#endif // STEERING_UTILITY_H
