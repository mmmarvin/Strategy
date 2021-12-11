#ifndef POSITION_COMMON_H
#define POSITION_COMMON_H

#include "../../core/type.h"
#include "../../core/vector2.h"

namespace strat
{
namespace position_common
{
  gengine2d::string_t getPositionString(const gengine2d::Vector2f& position,
                                        const gengine2d::Vector2f& spatialPosition);
  gengine2d::string_t getIsoPositionString(const gengine2d::Vector2f& isoPosition,
                                           const gengine2d::Vector2f& spatialPosition);
  gengine2d::string_t getCartPositionString(const gengine2d::Vector2f& cartPosition,
                                            const gengine2d::Vector2f& spatialPosition);
}
}

#endif // POSITION_COMMON_H
