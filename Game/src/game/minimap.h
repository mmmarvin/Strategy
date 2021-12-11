#ifndef MINIMAP_H
#define MINIMAP_H

#include <SFML/Graphics/RenderTexture.hpp>
#include "../../../core/game_internal_types.h"
#include "../../../core/vector2.h"
#include "player.h"

namespace strat
{
  class Map;
  class GameWorld;
  class Camera;
  class FogManager;

  class Minimap
  {
  public:
    Minimap(Map& map, const GameWorld& gameWorld, const Camera& camera, const FogManager& fogManager);
    bool init(unsigned int width, unsigned int height);

    void draw(Player::EPlayerNumber team, const gengine2d::Vector2<std::size_t>& mapSize) const;

    void setIsometric(bool isometric);

    gengine2d::game_internal::Renderer::RenderableTexture& getTexture() { return m_minimapTexture; }

  private:
    void drawFog(Player::EPlayerNumber team,
                 const gengine2d::Vector2<std::size_t>& mapSize,
                 float xRatio,
                 float yRatio) const;

    mutable gengine2d::game_internal::Renderer::RenderableTexture m_minimapTexture;

    Map*                                                          m_map;
    const GameWorld*                                              m_gameWorld;
    const Camera*                                                 m_camera;
    const FogManager*                                             m_fogManager;

    bool                                                          m_isoMetric;
  };
}

#endif // MINIMAP_H
