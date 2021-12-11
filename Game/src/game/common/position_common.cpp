#include "position_common.h"

#include "../../core/utility/stl_utility.h"
#include "../../core/utility/vector_utility.h"
#include "../game_def.h"

namespace strat
{
namespace position_common
{
  gengine2d::string_t getPositionString(const gengine2d::Vector2f& position,
                                        const gengine2d::Vector2f& spatialPosition)
  {
    return "cart: [" +
           gengine2d::stl_utils::to_string(static_cast<int>(position.x)) +
           ", " +
           gengine2d::stl_utils::to_string(static_cast<int>(position.y)) +
           "]\ngrid: [" +
           gengine2d::stl_utils::to_string(static_cast<int>(spatialPosition.x)) +
           ", " +
           gengine2d::stl_utils::to_string(static_cast<int>(spatialPosition.y)) +
           "]";
  }

  gengine2d::string_t getIsoPositionString(const gengine2d::Vector2f& isoPosition,
                                           const gengine2d::Vector2f& spatialPosition)
  {
    auto cartPosition = gengine2d::vector_utils::convertIsoToCartPoint(isoPosition);
    return "iso: [" +
           gengine2d::stl_utils::to_string(static_cast<int>(isoPosition.x)) +
           ", " +
           gengine2d::stl_utils::to_string(static_cast<int>(isoPosition.y)) +

           "]\ncart: [" +
           gengine2d::stl_utils::to_string(static_cast<int>(cartPosition.x)) +
           ", " +
           gengine2d::stl_utils::to_string(static_cast<int>(cartPosition.y)) +

           "]\ngrid: [" +
           gengine2d::stl_utils::to_string(static_cast<int>(spatialPosition.x)) +
           ", " +
           gengine2d::stl_utils::to_string(static_cast<int>(spatialPosition.y)) +
           "]";
  }

  gengine2d::string_t getCartPositionString(const gengine2d::Vector2f& cartPosition,
                                            const gengine2d::Vector2f& spatialPosition)
  {
    auto isoPosition = gengine2d::vector_utils::convertCartToIsoPoint(cartPosition);
    return "iso: [" +
           gengine2d::stl_utils::to_string(static_cast<int>(isoPosition.x)) +
           ", " +
           gengine2d::stl_utils::to_string(static_cast<int>(isoPosition.y)) +

           "]\ncart: [" +
           gengine2d::stl_utils::to_string(static_cast<int>(cartPosition.x)) +
           ", " +
           gengine2d::stl_utils::to_string(static_cast<int>(cartPosition.y)) +

           "]\ngrid: [" +
           gengine2d::stl_utils::to_string(static_cast<int>(spatialPosition.x)) +
           ", " +
           gengine2d::stl_utils::to_string(static_cast<int>(spatialPosition.y)) +
           "]";
  }
}
}
