#include "unitselector.h"

#include "../../core/collider/collision_test.h"
#include "../../core/collider/rectangle.h"
#include "../../core/debug/debug_log.h"
#include "../../core/game.h"
#include "../entity/component/unit_components.h"
#include "../utility/spatialpartition_utility.h"
#include "../game_def.h"

namespace strat
{
  UnitSelector::UnitSelector(const spatial_partition_t& spatialPartition) :
    m_spatialPartition(&spatialPartition)
  {
    for(auto& vertex : m_selectionVertex) {
      vertex.color = gengine2d::Color(255, 255, 0, 32);
    }
  }

  void UnitSelector::draw(bool showDebugSelectionGrid)
  {
    if(showDebugSelectionGrid && m_selectionTimer.is_running()) {
      if(m_selectionTimer.get_elapsed().as_seconds() > 3.f) {
        m_selectionTimer.stop();
      } else {
        gengine2d::GameEngine::getInstance().getRenderer().draw2DShape(m_selectionVertex.data(),
                                                                       m_selectionVertex.size(),
                                                                       nullptr);
      }
    }
  }

  anax::Entity UnitSelector::selectEntity(const gengine2d::Vector2f& isoPosition)
  {
    auto position = gengine2d::vector_utils::convertIsoToCartPoint(isoPosition);
    auto x = position.x / TILE_SIZE;
    auto y = position.y / TILE_SIZE;

    anax::Entity closestEntity;
    float closestIsoOrder = std::numeric_limits<float>::min();

    // TODO: Add selection to base collider too?
    m_spatialPartition->forEach(x - 1.f,
                                y - 1.f,
                                x + 1.f,
                                y + 1.f,
    [&isoPosition, &closestEntity, &closestIsoOrder ](const anax::Entity& otherEntity) {
      if(!base_component_utils::isOfType(otherEntity,
                                         component::BaseEntityType::EEntityType::Projectile) &&
         !base_component_utils::isOfType(otherEntity,
                                         component::BaseEntityType::EEntityType::Flag)) {
        auto otherPosition = gengine2d::vector_utils::convertCartToIsoPoint(otherEntity.getComponent<component::BasePosition>().position);
        const auto& otherSprite = otherEntity.getComponent<component::BaseSprite>().sprite;

        const auto& otherSpriteSize = otherSprite.getSize();
        gengine2d::Rectanglef spriteCollider(otherPosition.x - (otherSpriteSize.x / 2.f),
                                        otherPosition.y - otherSpriteSize.y,
                                        otherSpriteSize.x,
                                        otherSpriteSize.y);

        auto type = otherEntity.getComponent<component::BaseEntityType>().type;
        bool contains = gengine2d::contains(spriteCollider, isoPosition);
        if(contains &&
           (type == component::BaseEntityType::EEntityType::Unit ||
            type == component::BaseEntityType::EEntityType::Building ||
            type == component::BaseEntityType::EEntityType::Resource)) {

          auto currentIsoOrder = otherPosition.x + otherPosition.y;
          if(currentIsoOrder >= closestIsoOrder ) {
            closestEntity = otherEntity;
            closestIsoOrder  = currentIsoOrder;
          }
        }
      }
    });

    return closestEntity;
  }

  gengine2d::vector_t<anax::Entity> UnitSelector::selectEntities(Player::EPlayerNumber team,
                                                                 const gengine2d::Vector2f& isoSelectionStart,
                                                                 const gengine2d::Vector2f& isoSelectionEnd)
  {
    gengine2d::vector_t<anax::Entity> ret;

    auto dimension = isoSelectionEnd - isoSelectionStart;
    gengine2d::Rectanglef selector(isoSelectionStart.x, isoSelectionStart.y, dimension.x, dimension.y);

    auto [minRect, maxRect] = spatialpartition_utils::isometricForEach(isoSelectionStart,
                                                                       isoSelectionEnd,
                                                                       1.f,
                                                                       1.f,
                                                                       *m_spatialPartition,
    [team, &ret, &selector](const anax::Entity& otherEntity) {
      if(!base_component_utils::isOfType(otherEntity,
                                         component::BaseEntityType::EEntityType::Projectile) &&
         !base_component_utils::isOfType(otherEntity,
                                         component::BaseEntityType::EEntityType::Flag)) {
        auto position = gengine2d::vector_utils::convertCartToIsoPoint(otherEntity.getComponent<component::BasePosition>().position);
        const auto& collider = otherEntity.getComponent<component::BaseCollider>().collider;

        gengine2d::Circlef adjustedCollider(position.x, position.y, collider.radius);
        adjustedCollider.center -= collider.center;

        if(gengine2d::fullyContains(selector, adjustedCollider) &&
           otherEntity.hasComponent<component::UnitTeam>() &&
           otherEntity.getComponent<component::UnitTeam>().team == team) {
          ret.push_back(otherEntity);
        }
      }
    });

    minRect.x = floorf(minRect.x / TILE_SIZE);
    maxRect.x = floorf(maxRect.x / TILE_SIZE);
    minRect.y = floorf(minRect.y / TILE_SIZE);
    maxRect.y = floorf(maxRect.y / TILE_SIZE);
    auto sStart = gengine2d::Vector2f(minRect.x * TILE_SIZE, minRect.y * TILE_SIZE);
    auto sEnd = gengine2d::Vector2f(maxRect.x * TILE_SIZE, maxRect.y * TILE_SIZE);
    auto dim = sEnd - sStart;
    m_selectionVertex[0].position = sStart;
    m_selectionVertex[1].position = sStart + gengine2d::Vector2f(dim.x, 0.f);
    m_selectionVertex[2].position = sStart + gengine2d::Vector2f(0.f, dim.y);
    m_selectionVertex[3].position = m_selectionVertex[2].position;
    m_selectionVertex[4].position = sEnd;
    m_selectionVertex[5].position = m_selectionVertex[1].position;

    m_selectionVertex[0].position = gengine2d::vector_utils::convertCartToIsoPoint(m_selectionVertex[0].position);
    m_selectionVertex[1].position = gengine2d::vector_utils::convertCartToIsoPoint(m_selectionVertex[1].position);
    m_selectionVertex[2].position = gengine2d::vector_utils::convertCartToIsoPoint(m_selectionVertex[2].position);
    m_selectionVertex[3].position = gengine2d::vector_utils::convertCartToIsoPoint(m_selectionVertex[3].position);
    m_selectionVertex[4].position = gengine2d::vector_utils::convertCartToIsoPoint(m_selectionVertex[4].position);
    m_selectionVertex[5].position = gengine2d::vector_utils::convertCartToIsoPoint(m_selectionVertex[5].position);

    m_selectionTimer.restart();

    return ret;
  }
}
