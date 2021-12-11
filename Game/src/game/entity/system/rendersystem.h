#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SFML/Graphics.hpp>
#include <anax/anax.hpp>
#include "../../../core/styles/shape2dstyle.h"
#include "../../../core/styles/text2dstyle.h"
#include "../../fogmanager.h"
#include "../../player.h"
#include "../../spatialpartition.h"
#include "../component/base_components.h"
#include "../component/projectile_components.h"

namespace strat
{
  class GameWorld;
namespace system
{
  class RenderSystem
  {
    using minimap_render_system_type = anax::System<anax::Requires<component::BasePosition>,
                                                    anax::Excludes<component::ProjectileAttributes>>;

  public:
    RenderSystem(const spatial_partition_t& spatialPartition,
                 const FogManager& fogs);

    template<class ShadowDrawerFunc,
             class UnitStateDrawerFunc>
    void draw(float alpha,
              Player::EPlayerNumber team,
              ShadowDrawerFunc&& shadowDrawerFunc,
              UnitStateDrawerFunc&& unitStateDrawerFunc) const;

    template<class ShadowDrawerFunc,
             class UnitStateDrawerFunc>
    void drawWithHealth(float alpha,
                        Player::EPlayerNumber team,
                        ShadowDrawerFunc&& shadowDrawerFunc,
                        UnitStateDrawerFunc&& unitStateDrawerFunc) const;

    template<class PositionChangerFunc>
    void drawMiniMap(Player::EPlayerNumber team,
                     float xRatio,
                     float yRatio,
                     PositionChangerFunc&& positionChangerFunc) const;

  private:
    friend class ::strat::GameWorld;

    minimap_render_system_type& getMiniMapRenderSystem() { return m_minimapRenderSystem; }
    gengine2d::vector_t<anax::Entity> getEntitiesInDrawableArea(Player::EPlayerNumber team,
                                                                float minX,
                                                                float minY,
                                                                float maxX,
                                                                float maxY) const;

    template<class ShadowDrawerFunc,
             class PositionDrawerFunc>
    void drawEntity(const anax::Entity& entity,
                    const gengine2d::Vector2f& interpPosition,
                    Player::EPlayerNumber team,
                    ShadowDrawerFunc&& shadowDrawerFunc,
                    PositionDrawerFunc&& positionDrawerFunc) const;

    void drawHealthBar(Player::EPlayerNumber team,
                       const anax::Entity& entity,
                       const gengine2d::Vector2f& interpPosition,
                       const gengine2d::Vector2f& spriteSize) const;
    void drawResourceBar(const anax::Entity& entity,
                         const gengine2d::Vector2f& interpPosition,
                         const gengine2d::Vector2f& spriteSize) const;
    void drawBuildProgressBar(const anax::Entity& entity,
                              const gengine2d::Vector2f& interpPosition,
                              const gengine2d::Vector2f& spriteSize) const;

    minimap_render_system_type      m_minimapRenderSystem;

    mutable gengine2d::Shape2DStyle m_healthBarStyle;
    mutable gengine2d::Shape2DStyle m_healthBarBackgroundStyle;
    mutable gengine2d::Shape2DStyle m_resourceBarStyle;
    mutable gengine2d::Shape2DStyle m_resourceBarBackgroundStyle;
    mutable gengine2d::Shape2DStyle m_progressBarStyle;
    mutable gengine2d::Shape2DStyle m_progressBarBackgroundStyle;
    mutable gengine2d::Shape2DStyle m_colliderStyle;

    const spatial_partition_t*      m_spatialPartition;
    const FogManager*               m_fogs;
  };
}
}

#include "rendersystem.inl"

#endif // RENDERSYSTEM_H
