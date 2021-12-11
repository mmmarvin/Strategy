#ifndef POSITION_UTILITY_H
#define POSITION_UTILITY_H

#include "../../core/collider/circle.h"
#include "../../core/vector2.h"

namespace strat
{
namespace position_utils
{
  gengine2d::Vector2f clampEntityPosition(const gengine2d::Vector2f& position,
                                          const gengine2d::Circlef& collider,
                                          const gengine2d::Vector2f& mapSize);
}
}

#endif // POSITION_UTILITY_H
