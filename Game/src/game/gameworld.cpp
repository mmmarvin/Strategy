#include "gameworld.h"

#include "../../../core/styles/shape2dstyle.h"
#include "../../../core/utility/stl_utility.h"
#include "common/debug_common.h"
#include "common/position_common.h"
#include "game_def.h"

namespace strat
{
  GameWorld::GameWorld(const Map& map,
                       FogManager& fogs,
                       GlobalResource& globalResource) :
    m_entityManager(map, fogs),
    m_renderSystem(m_entityManager.m_spatialPartition, fogs),
    m_unitSystem(m_entityManager, globalResource, map, fogs),
    m_buildingUnitTrainSystem(m_entityManager),
    m_buildingAttackSystem(m_entityManager, map, fogs),
    m_projectileSystem(m_entityManager, map, fogs),
    m_projectileHomingSystem(m_entityManager, map, fogs)
  {
  }

  bool GameWorld::init()
  {
    auto& anaxWorld = m_entityManager.m_entityWorld;
    anaxWorld.addSystem(m_renderSystem.getMiniMapRenderSystem());
    anaxWorld.addSystem(m_unitSystem);
    anaxWorld.addSystem(m_unitSystem.getCastleFinderSystem());
    anaxWorld.addSystem(m_buildingUnitTrainSystem);
    anaxWorld.addSystem(m_buildingAttackSystem);
    anaxWorld.addSystem(m_projectileSystem);
    anaxWorld.addSystem(m_projectileHomingSystem);

    return true;
  }

  void GameWorld::draw(float alpha,
                       bool drawShadow,
                       bool drawHealth,
                       bool drawDebugPosition,
                       bool drawDebugBlastRadius) const
  {
    DISABLE_WARNING_BEGIN
    DISABLE_WARNING("-Wshadow")
    static constexpr auto shadowDrawer = [](const anax::Entity& entity,
                                            const gengine2d::Vector2f& interpPosition)
    {
       auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
       const auto& sprite = entity.getComponent<component::BaseSprite>().sprite;
       auto spriteSize = sprite.getSize() / 2.f;

       gengine2d::Shape2DStyle shadowStyle;
       shadowStyle.fillColor = gengine2d::Color(0, 0, 0, 128);

       renderer.draw2DCircle(interpPosition - spriteSize,
                             spriteSize.x,
                             shadowStyle);
    };

    static constexpr auto unitStateDrawer = [](const anax::Entity& entity,
                                               const gengine2d::Vector2f& interpPosition)
    {
      auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
      const auto& fontLookup = gengine2d::GameEngine::getInstance().getResourceManager().getFontLookup();
      const auto& spriteSize = entity.getComponent<component::BaseSprite>().sprite.getSize();
      const auto& spatialPosition = entity.getComponent<component::BaseSpatialPosition>().spatialPosition;

      auto positionString = position_common::getIsoPositionString(interpPosition,
                                                                  spatialPosition.getPosition());
      if(base_component_utils::isOfType(entity,
                                        component::BaseEntityType::EEntityType::Unit)) {
        positionString += gengine2d::string_t("\nState: ") +
                          string_converter_utils::to_string(unit_component_utils::getState(entity));
        if(unit_component_utils::isWorker(entity)) {
          const auto& unitCollectedResource = entity.getComponent<component::UnitWorkerResourceCollected>().collected;

          positionString += "\n";
          for(std::size_t i = 0; i < GlobalResource::EGlobalResouceType::GR_NUMBER_OF_RES; ++i) {
            positionString += string_converter_utils::to_string(static_cast<GlobalResource::EGlobalResouceType>(i)) +
                              ": " +
                              gengine2d::stl_utils::to_string(unitCollectedResource[i]) +
                              "\n";
          }
        }
      }

      gengine2d::Text2DStyle  stateStyle;
      stateStyle.font = &fontLookup.getResource("FONT_DEFAULT");
      stateStyle.fillColor = gengine2d::Color::White;
      stateStyle.outlineColor = gengine2d::Color::Black;
      stateStyle.outlineThickness = 1.f;
      stateStyle.characterSize = 12;
//      stateStyle.style = gengine2d::Text2DStyle::TS_BOLD;

      auto stateTextBounds = renderer.calculate2DTextBounds(positionString,
                                                            stateStyle);
      renderer.draw2DText(interpPosition -
                          gengine2d::Vector2<gengine2d::render::position_point_2d_t>((stateTextBounds.x / 2.f),
                                                                                     5.f + spriteSize.y +
                                                                                     stateTextBounds.y),
                          positionString,
                          stateStyle);
    };
    DISABLE_WARNING_END

    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    auto shadowDrawerFunc = drawShadow ? shadowDrawer : [](const anax::Entity&,
                                                           const gengine2d::Vector2f&) {};
    auto unitStateDrawerFunc = drawDebugPosition ? unitStateDrawer : [](const anax::Entity&,
                                                                        const gengine2d::Vector2f&) {};

    if(drawDebugBlastRadius) {
      m_projectileSystem.draw();
      m_projectileHomingSystem.draw();
    }

    // TODO: Change team
    if(drawHealth) {
      m_renderSystem.drawWithHealth(alpha,
                                    Player::EPlayerNumber::PN_PLAYER1,
                                    shadowDrawerFunc,
                                    unitStateDrawerFunc);
    } else {
      m_renderSystem.draw(alpha,
                          Player::EPlayerNumber::PN_PLAYER1,
                          shadowDrawerFunc,
                          unitStateDrawerFunc);
    }
  }

  void GameWorld::update(float dt)
  {
    m_projectileSystem.update(dt);
    m_projectileHomingSystem.update(dt);
    m_unitSystem.update(dt);
    m_buildingUnitTrainSystem.update();
    m_buildingAttackSystem.update();

    m_entityManager.refresh();
  }
}
