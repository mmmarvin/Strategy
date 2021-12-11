#include "spatialpartition_utility.h"

#include "../../core/utility/float_utility.h"
#include "../game_def.h"

namespace strat
{
namespace spatialpartition_utils
{
namespace spatialpartition_utils_detail
{
  void pollClosestEntity(const anax::Entity& entity,
                         const gengine2d::Vector2f& pointClosestTo,
                         anax::Entity& closestEntity,
                         float& closestDistance)
  {
    auto d = gengine2d::distance(pointClosestTo,
                                 entity.getComponent<component::BasePosition>().position);
    if(d <= closestDistance) {
      closestEntity = entity;
      closestDistance = d;
    }
  }
}
  void updateSpatialPosition(const anax::Entity& entity,
                             const gengine2d::Vector2f& position,
                             detail::SpatialParitionAnaxObjectHandle& spatialPosition,
                             spatial_partition_t& spatialPartition)
  {
    auto posX = floorf(position.x / TILE_SIZE);
    auto posY = floorf(position.y / TILE_SIZE);
    const auto& spv = spatialPosition.getPosition();

    if(!gengine2d::float_utils::float_equal(posX, spv.x) ||
       !gengine2d::float_utils::float_equal(posY, spv.y)) {
      spatialPartition.remove(spatialPosition);
      spatialPosition = spatialPartition.add(posX, posY, entity);
    }
  }
}
}
