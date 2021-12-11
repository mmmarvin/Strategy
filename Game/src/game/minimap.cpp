#include "minimap.h"

#include "map/map.h"
#include "camera.h"
#include "gameworld.h"

namespace strat
{
  Minimap::Minimap(Map& map, const GameWorld& gameWorld, const Camera& camera, const FogManager& fogManager) :
    m_map(&map),
    m_gameWorld(&gameWorld),
    m_camera(&camera),
    m_fogManager(&fogManager),
    m_isoMetric(false)
  {
  }

  bool Minimap::init(unsigned int width, unsigned int height)
  {
    if(!m_minimapTexture.create(width, height)) {
      return false;
    }

    return true;
  }

  void Minimap::setIsometric(bool isometric)
  {
    if(m_isoMetric != isometric) {
      m_isoMetric = isometric;
      if(m_isoMetric) {
        m_map->setMinimapToIsometric();
      } else {
        m_map->setMinimapToCartesian();
      }
    }
  }

  void Minimap::draw(Player::EPlayerNumber team, const gengine2d::Vector2<std::size_t>& mapSize) const
  {
    static constexpr auto defaultPositionChanger = [](const gengine2d::Vector2f& v) { return v; };
    static constexpr auto isometricPositionChanger = [](const gengine2d::Vector2f& v) { return gengine2d::vector_utils::convertCartToIsoPoint(v); };
    static constexpr auto cartPositionChanger = [](const gengine2d::Vector2f& v) { return gengine2d::vector_utils::convertIsoToCartPoint(v); };

    auto xRatio = MINIMAP_SIZE / (mapSize.x * TILE_SIZE);
    auto yRatio = MINIMAP_SIZE / (mapSize.y * TILE_SIZE);

    gengine2d::GameEngine::getInstance().getRenderer().setRenderTargetToTexture(m_minimapTexture);
    m_minimapTexture.clear(gengine2d::Color::Transparent);

    if(m_isoMetric) {
      m_map->drawMiniMap();
      m_gameWorld->drawMiniMap(team,
                               xRatio,
                               yRatio,
                               isometricPositionChanger);
      drawFog(team, mapSize, xRatio, yRatio);
      m_camera->drawMiniMap(xRatio,
                            yRatio,
                            defaultPositionChanger);
    } else {
      m_map->drawMiniMap();
      m_gameWorld->drawMiniMap(team,
                               xRatio,
                               yRatio,
                               defaultPositionChanger);
      drawFog(team, mapSize, xRatio, yRatio);
      m_camera->drawMiniMap(xRatio,
                            yRatio,
                            cartPositionChanger);
    }

    m_minimapTexture.display();
    gengine2d::GameEngine::getInstance().getRenderer().resetRenderTarget();
  }

  void Minimap::drawFog(Player::EPlayerNumber team,
                        const gengine2d::Vector2<std::size_t>& mapSize,
                        float xRatio,
                        float yRatio) const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    auto minimapTileWidth = TILE_SIZE * xRatio;
    auto minimapTileHeight = TILE_SIZE * yRatio;

    const auto& fog = m_fogManager->getFog(team);
    gengine2d::array_t<gengine2d::Vertex2, 6> fogSprite;
    fogSprite[0].color = gengine2d::Color(0, 0, 0);
    fogSprite[1].color = gengine2d::Color(0, 0, 0);
    fogSprite[2].color = gengine2d::Color(0, 0, 0);
    fogSprite[3].color = gengine2d::Color(0, 0, 0);
    fogSprite[4].color = gengine2d::Color(0, 0, 0);
    fogSprite[5].color = gengine2d::Color(0, 0, 0);

    gengine2d::vector_t<gengine2d::Vertex2> fogVertices;
    fogVertices.reserve(mapSize.x * mapSize.y);

    for(float x = 0, maxX = mapSize.x; x < maxX; x += 1.f) {
      for(float y = 0, maxY = mapSize.y; y < maxY; y += 1.f) {
        if(!fog.isExposed(x, y)) {
          fogSprite[0].position = gengine2d::Vector2f(x * minimapTileWidth, y * minimapTileHeight);
          fogSprite[1].position = fogSprite[0].position + gengine2d::Vector2f(minimapTileWidth, 0.f);
          fogSprite[2].position = fogSprite[0].position + gengine2d::Vector2f(0.f, minimapTileHeight);

          fogSprite[3].position = fogSprite[2].position;
          fogSprite[4].position = fogSprite[0].position + gengine2d::Vector2f(minimapTileWidth,
                                                                              minimapTileHeight);
          fogSprite[5].position = fogSprite[1].position;

          fogVertices.insert(fogVertices.end(), fogSprite.begin(), fogSprite.end());
        }
      }
    }

    renderer.draw2DShape(fogVertices.data(), fogVertices.size(), nullptr);
  }
}
