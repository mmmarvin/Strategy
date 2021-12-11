#ifndef SPATIALPARTITION_UTILITY_H
#define SPATIALPARTITION_UTILITY_H

#include <anax/anax.hpp>
#include "../spatialpartition.h"

namespace strat
{
namespace spatialpartition_utils
{
namespace spatialpartition_utils_detail
{
  void pollClosestEntity(const anax::Entity& entity,
                         const gengine2d::Vector2f& pointClosestTo,
                         anax::Entity& closestEntity,
                         float& closestDistance);
}
  void updateSpatialPosition(const anax::Entity& entity,
                             const gengine2d::Vector2f& position,
                             detail::SpatialParitionAnaxObjectHandle& spatialPosition,
                             spatial_partition_t& spatialPartition);

  template<class Func>
  anax::Entity findClosestEntity(float minX,
                                 float minY,
                                 float maxX,
                                 float maxY,
                                 const gengine2d::Vector2f& position,
                                 const spatial_partition_t& spatialPartition,
                                 Func&& func);

  template<class Func>
  anax::Entity findEntityExpandingFromPoint(float x,
                                            float y,
                                            float size,
                                            const spatial_partition_t& spatialPartition,
                                            Func&& func);

  template<class Func>
  std::pair<gengine2d::Vector2f, gengine2d::Vector2f>  isometricForEach(const gengine2d::Vector2f& isoRectMin,
                                                                        const gengine2d::Vector2f& isoRectMax,
                                                                        float extraX,
                                                                        float extraY,
                                                                        const spatial_partition_t& spatialPartition,
                                                                        Func&& func);
}
}

#include "spatialpartition_utility.inl"

#endif // SPATIALPARTITION_UTILITY_H
