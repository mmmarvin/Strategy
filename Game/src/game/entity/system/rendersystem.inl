#include "rendersystem.h"

#include "../../../core/game.h"
#include "../../../core/utility/shape_utility.h"
#include "../../../core/utility/vector_utility.h"
#include "../../game_def.h"
#include "../component/base_components.h"
#include "../component/building_components.h"
#include "../component/unit_components.h"
#include "common/minimap_color_scheme.h"

namespace strat
{
namespace system
{
  template<class ShadowDrawerFunc,
           class UnitStateDrawerFunc>
  void RenderSystem::draw(float alpha,
                          Player::EPlayerNumber team,
                          ShadowDrawerFunc&& shadowDrawerFunc,
                          UnitStateDrawerFunc&& unitStateDrawerFunc) const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    const auto& gameView = gengine2d::GameEngine::getInstance().getRenderer().get2DCamera();

    auto drawableArea = gengine2d::shape_utils::getRectPoints(gameView.getCenter(), gameView.getSize());
    auto entities = getEntitiesInDrawableArea(team,
                                              drawableArea[0],
                                              drawableArea[1],
                                              drawableArea[2],
                                              drawableArea[3]);

    for(const auto& entity : entities) {
      const auto& oldPosition = entity.getComponent<component::BasePosition>().oldPosition;
      const auto& position = entity.getComponent<component::BasePosition>().position;
      auto interpPosition = gengine2d::vector_utils::convertCartToIsoPoint(gengine2d::linterp(oldPosition,
                                                                                              position,
                                                                                              alpha));

      drawEntity(entity,
                 interpPosition,
                 team,
                 std::forward<ShadowDrawerFunc>(shadowDrawerFunc),
                 std::forward<UnitStateDrawerFunc>(unitStateDrawerFunc));
    }
  }

  template<class ShadowDrawerFunc,
           class UnitStateDrawerFunc>
  void RenderSystem::drawWithHealth(float alpha,
                                    Player::EPlayerNumber team,
                                    ShadowDrawerFunc&& shadowDrawerFunc,
                                    UnitStateDrawerFunc&& unitStateDrawerFunc) const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    const auto& gameView = gengine2d::GameEngine::getInstance().getRenderer().get2DCamera();

    auto drawableArea = gengine2d::shape_utils::getRectPoints(gameView.getCenter(), gameView.getSize());
    auto entities = getEntitiesInDrawableArea(team,
                                              drawableArea[0],
                                              drawableArea[1],
                                              drawableArea[2],
                                              drawableArea[3]);

    for(const auto& entity : entities) {
      const auto& oldPosition = entity.getComponent<component::BasePosition>().oldPosition;
      const auto& position = entity.getComponent<component::BasePosition>().position;
      auto interpPosition = gengine2d::vector_utils::convertCartToIsoPoint(gengine2d::linterp(oldPosition,
                                                                                              position,
                                                                                              alpha));

      drawEntity(entity,
                 interpPosition,
                 team,
                 std::forward<ShadowDrawerFunc>(shadowDrawerFunc),
                 std::forward<UnitStateDrawerFunc>(unitStateDrawerFunc));

      auto type = entity.getComponent<component::BaseEntityType>().type;
      switch(type) {
      case component::BaseEntityType::EEntityType::Unit: {
        const auto& sprite = entity.getComponent<component::BaseSprite>().sprite;
        const auto& spriteSize = sprite.getSize();
        drawHealthBar(team, entity, interpPosition, spriteSize);
        break;
      }
      case component::BaseEntityType::EEntityType::Building: {
        const auto& sprite = entity.getComponent<component::BaseSprite>().sprite;
        const auto& spriteSize = sprite.getSize();
        if(entity.getComponent<component::BuildingCompleteness>().completed) {
          drawHealthBar(team, entity, interpPosition, spriteSize);
        } else {
          drawBuildProgressBar(entity, interpPosition, spriteSize);
        }
        break;
      }
      case component::BaseEntityType::EEntityType::Resource: {
        const auto& sprite = entity.getComponent<component::BaseSprite>().sprite;
        const auto& spriteSize = sprite.getSize();
        drawResourceBar(entity, interpPosition, spriteSize);
        break;
      }
      default:
        break;
      }
    }
  }

  template<class PositionChangerFunc>
  void RenderSystem::drawMiniMap(Player::EPlayerNumber team,
                                 float xRatio,
                                 float yRatio,
                                 PositionChangerFunc&& positionChangerFunc) const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    const auto& minimapColorScheme = system_common::getMinimapColorScheme();

    gengine2d::Shape2DStyle entityStyle;

    for(const auto& entity : m_minimapRenderSystem.getEntities()) {
      const auto& spatialPosition = base_component_utils::getSpatialPositionHandle(entity);
      if(m_fogs->getFog(team).isExposed(spatialPosition.getPosition().x,
                                      spatialPosition.getPosition().y)) {
        if(!base_component_utils::isOfType(entity, component::BaseEntityType::EEntityType::Flag) &&
           !base_component_utils::isOfType(entity, component::BaseEntityType::EEntityType::Projectile)) {
          auto position = std::forward<PositionChangerFunc>(positionChangerFunc)(entity.getComponent<component::BasePosition>().position);
          auto adjustedXPosition = position.x * xRatio;
          auto adjustedYPosition = position.y * yRatio;

          float entitySizeX, entitySizeY;
          if(base_component_utils::isOfType(entity,component::BaseEntityType::EEntityType::Unit)) {
            auto entityTeam = entity.getComponent<component::UnitTeam>().team;
            entitySizeX = MINIMAP_UNIT_SIZE * xRatio * (TILE_SIZE / 2.f);
            entitySizeY = MINIMAP_UNIT_SIZE * yRatio * (TILE_SIZE / 2.f);

            entityStyle.fillColor = minimapColorScheme[entityTeam];
          } else if(base_component_utils::isOfType(entity,
                                                   component::BaseEntityType::EEntityType::Building)) {
            auto entityTeam = entity.getComponent<component::UnitTeam>().team;
            entitySizeX = MINIMAP_BUILDING_SIZE * xRatio * (TILE_SIZE / 2.f);
            entitySizeY = MINIMAP_BUILDING_SIZE * yRatio * (TILE_SIZE / 2.f);

            entityStyle.fillColor = minimapColorScheme[entityTeam];
          } else if(base_component_utils::isOfType(entity,
                                                   component::BaseEntityType::EEntityType::Resource)) {
            entitySizeX = MINIMAP_RESOURCE_SIZE * xRatio * (TILE_SIZE / 2.f);
            entitySizeY = MINIMAP_RESOURCE_SIZE * yRatio * (TILE_SIZE / 2.f);

            entityStyle.fillColor = gengine2d::Color(100, 100, 100);
          }

          renderer.draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(adjustedXPosition - (entitySizeX / 2.f),
                                                                                      adjustedYPosition - (entitySizeY / 2.f),
                                                                                      entitySizeX,
                                                                                      entitySizeY),
                              entityStyle);
        }
      }
    }
  }

  template<class ShadowDrawerFunc,
           class UnitStateDrawerFunc>
  void RenderSystem::drawEntity(const anax::Entity& entity,
                                const gengine2d::Vector2f& interpPosition,
                                Player::EPlayerNumber team,
                                ShadowDrawerFunc&& shadowDrawerFunc,
                                UnitStateDrawerFunc&& unitStateDrawerFunc) const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    auto& sprite = entity.getComponent<component::BaseSprite>().sprite;
    auto type = entity.getComponent<component::BaseEntityType>().type;

    if(entity.hasComponent<component::BaseFlags>()) {
      auto isSelected = entity.getComponent<component::BaseFlags>().flags[component::BaseFlags::BF_FLAG_IS_SELECTED];
      if(isSelected) {
        const auto& baseCollider = entity.getComponent<component::BaseCollider>().collider;
        auto colliderRadius = baseCollider.radius + 5.f;

        if(type == component::BaseEntityType::EEntityType::Unit ||
           type == component::BaseEntityType::EEntityType::Building) {
          if(entity.getComponent<component::UnitTeam>().team == team) {
            auto colliderPosition = gengine2d::round(interpPosition - baseCollider.center);
            colliderPosition -= gengine2d::Vector2f(colliderRadius, colliderRadius);

            renderer.draw2DCircle(gengine2d::Vector2<gengine2d::render::position_point_2d_t>(colliderPosition.x,
                                                                                             colliderPosition.y),
                                  colliderRadius,
                                  m_colliderStyle);
          }
        }
      }
    }

    std::forward<ShadowDrawerFunc>(shadowDrawerFunc)(entity, interpPosition);

    auto spritePosition = gengine2d::round(interpPosition - gengine2d::Vector2f(sprite.getSize().x / 2.f,
                                                                                sprite.getSize().y));
    renderer.draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(spritePosition.x,
                                                                                spritePosition.y,
                                                                                sprite.getSize().x,
                                                                                sprite.getSize().y),
                        gengine2d::Shape2DStyle());

    std::forward<UnitStateDrawerFunc>(unitStateDrawerFunc)(entity, interpPosition);
  }
}
}
