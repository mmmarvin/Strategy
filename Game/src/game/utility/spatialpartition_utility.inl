#include "spatialpartition_utility.h"

#include "../../core/utility/collider_utility.h"
#include "../../core/utility/shape_utility.h"
#include "../../core/utility/vector_utility.h"
#include "../entity/component/base_components.h"
#include "../game_def.h"

namespace strat
{
namespace spatialpartition_utils
{
namespace spatialpartition_utils_detail
{
  template<class Func>
  std::pair<anax::Entity, float> searchInX(float x,
                                           float y,
                                           float maxX,
                                           const spatial_partition_t& spatialPartition,
                                           Func&& func)
  {
    anax::Entity closestEntity;
    float closestDistance = std::numeric_limits<float>::max();

    for(auto curX = x; curX <= maxX; curX += 1.f) {
      auto entities = spatialPartition.getSingle(curX, y);
      for(const auto* entity : entities) {
        if(std::forward<Func>(func)(*entity)) {
          spatialpartition_utils_detail::pollClosestEntity(*entity,
                                                           gengine2d::Vector2f(x, y),
                                                           closestEntity,
                                                           closestDistance);
        }
      }
    }

    return std::make_pair(closestEntity, closestDistance);
  }

  template<class Func>
  std::pair<anax::Entity, float> searchInY(float x,
                                           float y,
                                           float maxY,
                                           const spatial_partition_t& spatialPartition,
                                           Func&& func)
  {
    anax::Entity closestEntity;
    float closestDistance = std::numeric_limits<float>::max();

    for(auto curY = y; curY <= maxY; curY += 1.f) {
      auto entities = spatialPartition.getSingle(x, curY);
      for(const auto* entity : entities) {
        if(std::forward<Func>(func)(*entity)) {
          spatialpartition_utils_detail::pollClosestEntity(*entity,
                                                           gengine2d::Vector2f(x, y),
                                                           closestEntity,
                                                           closestDistance);
        }
      }
    }

    return std::make_pair(closestEntity, closestDistance);
  }
}
  template<class Func>
  anax::Entity findClosestEntity(float minX,
                                 float minY,
                                 float maxX,
                                 float maxY,
                                 const gengine2d::Vector2f& position,
                                 const spatial_partition_t& spatialPartition,
                                 Func&& func)
  {
    anax::Entity closestEntity;
    float closestDistance = std::numeric_limits<float>::max();

    spatialPartition.forEach(minX, minY, maxX, maxY,
    [&position, &func, &closestEntity, &closestDistance](const anax::Entity& otherEntity) {
      if(std::forward<Func>(func)(otherEntity)) {
        const auto& otherPosition = otherEntity.getComponent<component::BasePosition>().position;
        auto d = gengine2d::distance(position, otherPosition);
        if(d <= closestDistance) {
          closestEntity = otherEntity;
          closestDistance = d;
        }
      }
    });

    return closestEntity;
  }

  template<class Func>
  anax::Entity findEntityExpandingFromPoint(float x,
                                            float y,
                                            float size,
                                            const spatial_partition_t& spatialPartition,
                                            Func&& func)
  {
    auto lastX = x - size;
    auto lowerX = x, upperX = x;
    auto lowerY = y, upperY = y;

    anax::Entity closestEntity;
    float closestDistance = std::numeric_limits<float>::max();

    // first search the point
    auto entities = spatialPartition.getSingle(x, y);
    for(const auto* entity : entities) {
      if(std::forward<Func>(func)(*entity)) {
        spatialpartition_utils_detail::pollClosestEntity(*entity,
                                                         gengine2d::Vector2f(x, y),
                                                         closestEntity,
                                                         closestDistance);
      }
    }

    if(closestEntity.isValid()) {
      return closestEntity;
    }

    // decrease starting point, and increase end points for each coordinate
    lowerX -= 1.f;
    upperX += 1.f;
    lowerY -= 1.f;
    upperY += 1.f;

    while(lowerX >= lastX) {
      // look from [minx, miny] -> [maxx, miny]
      auto closestEntityPair = spatialpartition_utils_detail::searchInX(lowerX,
                                                                        lowerY,
                                                                        upperX,
                                                                        spatialPartition,
                                                                        std::forward<Func>(func));
      if(closestEntityPair.first.isValid() &&
         closestEntityPair.second <= closestDistance) {
        closestEntity = closestEntity;
        closestDistance = closestEntityPair.second;
      }

      // look from [minx, miny] -> [minx, maxy]
      closestEntityPair = spatialpartition_utils_detail::searchInY(lowerX,
                                                                   lowerY,
                                                                   upperY,
                                                                   spatialPartition,
                                                                   std::forward<Func>(func));
      if(closestEntityPair.first.isValid() &&
         closestEntityPair.second <= closestDistance) {
        closestEntity = closestEntity;
        closestDistance = closestEntityPair.second;
      }

      // look from [minx, maxy] -> [maxx, maxy]
      closestEntityPair = spatialpartition_utils_detail::searchInX(lowerX,
                                                                   upperY,
                                                                   upperX,
                                                                   spatialPartition,
                                                                   std::forward<Func>(func));
      if(closestEntityPair.first.isValid() &&
         closestEntityPair.second <= closestDistance) {
        closestEntity = closestEntity;
        closestDistance = closestEntityPair.second;
      }

      // look from [maxx, miny] -> [maxx, maxy]
      closestEntityPair = spatialpartition_utils_detail::searchInY(upperX,
                                                                   lowerY,
                                                                   upperY,
                                                                   spatialPartition,
                                                                   std::forward<Func>(func));
      if(closestEntityPair.first.isValid() &&
         closestEntityPair.second <= closestDistance) {
        closestEntity = closestEntity;
        closestDistance = closestEntityPair.second;
      }

      if(closestEntity.isValid()) {
        return closestEntity;
      }

      lowerX -= 1.f;
      upperX += 1.f;
      lowerY -= 1.f;
      upperY += 1.f;
    }

    return anax::Entity();
  }

  template<class Func>
  std::pair<gengine2d::Vector2f, gengine2d::Vector2f> isometricForEach(const gengine2d::Vector2f& isoRectMin,
                                                                       const gengine2d::Vector2f& isoRectMax,
                                                                       float extraX,
                                                                       float extraY,
                                                                       const spatial_partition_t& spatialPartition,
                                                                       Func&& func)
  {
    auto isoRect = gengine2d::collider_utils::generateRect(isoRectMin, isoRectMax);
    auto quadPoints = gengine2d::shape_utils::getRectPoints(isoRect);

    quadPoints[0] = gengine2d::vector_utils::convertIsoToCartPoint(quadPoints[0]);
    quadPoints[1] = gengine2d::vector_utils::convertIsoToCartPoint(quadPoints[1]);
    quadPoints[2] = gengine2d::vector_utils::convertIsoToCartPoint(quadPoints[2]);
    quadPoints[3] = gengine2d::vector_utils::convertIsoToCartPoint(quadPoints[3]);

    auto [cartRectMin, cartRectMax] = gengine2d::shape_utils::quadToTightFitRect(quadPoints);
    auto minX = cartRectMin.x / TILE_SIZE;
    auto minY = cartRectMin.y / TILE_SIZE;
    auto maxX = cartRectMax.x / TILE_SIZE;
    auto maxY = cartRectMax.y / TILE_SIZE;
    spatialPartition.forEach(minX - extraX,
                             minY - extraY,
                             maxX + extraX + 1,
                             maxY + extraY + 1,
                             std::forward<Func>(func));

    return std::make_pair(cartRectMin - gengine2d::Vector2f(extraX * TILE_SIZE, extraY * TILE_SIZE),
                          cartRectMax + gengine2d::Vector2f((extraX + 1.f) * TILE_SIZE,
                                                            (extraY + 1.f) * TILE_SIZE));
  }
}
}
