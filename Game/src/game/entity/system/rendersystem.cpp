#include "rendersystem.h"

#include "../../../core/debug/debug_log.h"
#include "../../../core/game.h"
#include "../../utility/spatialpartition_utility.h"
#include "../component/flag_components.h"
#include "../component/resource_components.h"

namespace strat
{
namespace system
{
  RenderSystem::RenderSystem(const spatial_partition_t& spatialPartition,
                             const FogManager& fogs) :
    m_spatialPartition(&spatialPartition),
    m_fogs(&fogs)
  {
    m_colliderStyle.outlineThickness = 2;
    m_colliderStyle.fillColor = gengine2d::Color(0, 255, 0, 64);
    m_colliderStyle.outlineColor = gengine2d::Color(0, 255, 0, 255);
    m_healthBarStyle.fillColor = gengine2d::Color::Red;
    m_resourceBarStyle.fillColor = gengine2d::Color::Yellow;
    m_resourceBarBackgroundStyle.fillColor = gengine2d::Color::Black;
    m_progressBarStyle.fillColor = gengine2d::Color(0, 128, 128, 255);
    m_progressBarBackgroundStyle.fillColor = gengine2d::Color(100, 100, 100);
  }

  gengine2d::vector_t<anax::Entity> RenderSystem::getEntitiesInDrawableArea(Player::EPlayerNumber team,
                                                                            float minX,
                                                                            float minY,
                                                                            float maxX,
                                                                            float maxY) const
  {
    gengine2d::vector_t<anax::Entity> entities;
    const auto* fogs = m_fogs;
    spatialpartition_utils::isometricForEach(gengine2d::Vector2f(minX, minY),
                                             gengine2d::Vector2f(maxX, maxY),
                                             1.f,
                                             1.f,
                                             *m_spatialPartition,
    [team, &entities, &fogs](const anax::Entity& entity) {
      if(entity.isValid() &&
         base_component_utils::isOfType(entity,
                                        component::BaseEntityType::EEntityType::Flag)) {
        if(entity.hasComponent<component::FlagTimer>()) {
          const auto& flagTimer = flag_component_utils::getTimer(entity);
          if(flagTimer.is_running() && flagTimer.get_elapsed().as_seconds() < 3.f) {
            entities.push_back(entity);
          }
        } else {
          if(unit_component_utils::getTeam(entity) == team &&
             base_component_utils::getFlag(flag_component_utils::getOwner(entity),
                                           component::BaseFlags::BF_FLAG_IS_SELECTED)) {
            entities.push_back(entity);
          }
        }
      } else {
        const auto& spatialPosition = base_component_utils::getSpatialPositionHandle(entity);
        if(fogs->getFog(team).isExposed(spatialPosition.getPosition().x,
                                        spatialPosition.getPosition().y)) {
          entities.push_back(entity);
        }
      }
    });

    std::sort(entities.begin(), entities.end(), [](const anax::Entity& lhs, const anax::Entity& rhs) {
      const auto& lhsPos = lhs.getComponent<component::BasePosition>().position;
      const auto& rhsPos = rhs.getComponent<component::BasePosition>().position;

      return /*lhsPos.x < rhsPos.x ||*/
             lhsPos.y < rhsPos.y;
    });

    return entities;
  }

  void RenderSystem::drawHealthBar(Player::EPlayerNumber team,
                                   const anax::Entity& entity,
                                   const gengine2d::Vector2f& interpPosition,
                                   const gengine2d::Vector2f& spriteSize) const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    const auto& baseAttribs = entity.getComponent<component::BaseAttributes>().attributes;
    auto currHp = baseAttribs[component::BaseAttributes::BA_ATTRIB_HP];
    auto maxHp = baseAttribs[component::BaseAttributes::BA_ATTRIB_MAX_HP];
    auto ratio = currHp / maxHp;

    auto healthBarPosition = interpPosition - gengine2d::Vector2f(16.f,
                                                                  spriteSize.y + 20.f);

    assert(entity.hasComponent<component::UnitTeam>());
    if(entity.getComponent<component::UnitTeam>().team != team) {
      m_healthBarStyle.fillColor = gengine2d::Color::Red;
      m_healthBarBackgroundStyle.fillColor = gengine2d::Color::Black;
    } else {
      m_healthBarStyle.fillColor = gengine2d::Color::Green;
      m_healthBarBackgroundStyle.fillColor = gengine2d::Color::Red;
    }

    renderer.draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(healthBarPosition.x,
                                                                                healthBarPosition.y,
                                                                                32,
                                                                                10),
                        m_healthBarBackgroundStyle);
    renderer.draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(healthBarPosition.x,
                                                                                healthBarPosition.y,
                                                                                32 * ratio,
                                                                                10),
                        m_healthBarStyle);
  }

  void RenderSystem::drawResourceBar(const anax::Entity& entity,
                                     const gengine2d::Vector2f& interpPosition,
                                     const gengine2d::Vector2f& spriteSize) const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    const auto& resAttribs = entity.getComponent<component::ResourceAttributes>().attributes;
    float currResAvail = resAttribs[component::ResourceAttributes::RA_ATTRIB_RES_VALUE_LEFT];
    float maxResAvail = resAttribs[component::ResourceAttributes::RA_ATTRIB_RES_VALUE_MAX];
    auto ratio = currResAvail / maxResAvail;

    auto resourceBarPosition = interpPosition - gengine2d::Vector2f(32.f,
                                                                    spriteSize.y + 20.f);

    renderer.draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(resourceBarPosition.x,
                                                                                resourceBarPosition.y,
                                                                                64,
                                                                                10),
                        m_resourceBarBackgroundStyle);
    renderer.draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(resourceBarPosition.x,
                                                                                resourceBarPosition.y,
                                                                                64 * ratio,
                                                                                10),
                        m_resourceBarStyle);
  }

  void RenderSystem::drawBuildProgressBar(const anax::Entity& entity,
                                          const gengine2d::Vector2f& interpPosition,
                                          const gengine2d::Vector2f& spriteSize) const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    const auto& baseAttribs = entity.getComponent<component::BaseAttributes>().attributes;
    auto currHp = baseAttribs[component::BaseAttributes::BA_ATTRIB_HP];
    auto maxHp = baseAttribs[component::BaseAttributes::BA_ATTRIB_MAX_HP];
    auto ratio = currHp / maxHp;

    auto progressBarPosition = interpPosition - gengine2d::Vector2f(32.f,
                                                                    spriteSize.y + 20.f);

    assert(base_component_utils::isOfType(entity, component::BaseEntityType::EEntityType::Building));

    renderer.draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(progressBarPosition.x,
                                                                                progressBarPosition.y,
                                                                                64,
                                                                                10),
                        m_progressBarBackgroundStyle);
    renderer.draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(progressBarPosition.x,
                                                                                progressBarPosition.y,
                                                                                64 * ratio,
                                                                                10),
                        m_progressBarStyle);
  }
}
}
