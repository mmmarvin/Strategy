#include "fog.h"

#include "../core/game.h"
#include "../core/utility/collider_utility.h"
#include "../core/utility/shape_utility.h"
#include "../core/utility/vector_utility.h"
#include "game_def.h"

namespace strat
{
  bool Fog::init(std::size_t width, std::size_t height, std::uint8_t alpha)
  {
    for(std::size_t j = 0; j < height; ++j) {
      for(std::size_t i = 0; i < width; ++i) {
        m_fog.emplace(std::make_pair(static_cast<float>(i),
                                     static_cast<float>(j)), 1);
      }
    }

    return true;
  }

  void Fog::draw()
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    const auto& view = renderer.get2DCamera();

    auto drawableArea = gengine2d::shape_utils::getRectPoints(view.getCenter(), view.getSize());
    auto isoRect = gengine2d::collider_utils::generateRect(gengine2d::Vector2f(drawableArea[0],
                                                                               drawableArea[1]),
                                                           gengine2d::Vector2f(drawableArea[2],
                                                                               drawableArea[3]));
    auto quadPoints = gengine2d::shape_utils::getRectPoints(isoRect);

    quadPoints[0] = gengine2d::vector_utils::convertIsoToCartPoint(quadPoints[0]);
    quadPoints[1] = gengine2d::vector_utils::convertIsoToCartPoint(quadPoints[1]);
    quadPoints[2] = gengine2d::vector_utils::convertIsoToCartPoint(quadPoints[2]);
    quadPoints[3] = gengine2d::vector_utils::convertIsoToCartPoint(quadPoints[3]);

    auto [cartRectMin, cartRectMax] = gengine2d::shape_utils::quadToTightFitRect(quadPoints);
    auto minX = floorf(cartRectMin.x / TILE_SIZE);
    auto minY = floorf(cartRectMin.y / TILE_SIZE);
    auto maxX = floorf(cartRectMax.x / TILE_SIZE);
    auto maxY = floorf(cartRectMax.y / TILE_SIZE);

    gengine2d::vector_t<gengine2d::Vertex2> fogVertices;
    fogVertices.reserve((maxY - minY) * (maxX - minX)); // reserve enough to fill the screen with fog

    gengine2d::array_t<gengine2d::Vertex2, 6> fogVertex;
    fogVertex[0].color = gengine2d::Color::Black;
    fogVertex[1].color = gengine2d::Color::Black;
    fogVertex[2].color = gengine2d::Color::Black;
    fogVertex[3].color = gengine2d::Color::Black;
    fogVertex[4].color = gengine2d::Color::Black;
    fogVertex[5].color = gengine2d::Color::Black;

    for(float y = minY; y <= maxY; y += 1.f) {
      for(float x = minX; x <= maxX; x += 1.f) {
        auto position = std::make_pair(x, y);
        auto findIt = m_fog.find(position);
        if(findIt != m_fog.end() && findIt->second > 0) {
          auto cartSpritePosition = gengine2d::Vector2f(x * TILE_SIZE,
                                                        y * TILE_SIZE);

          fogVertex[0].position = gengine2d::vector_utils::convertCartToIsoPoint(cartSpritePosition);
          fogVertex[1].position = gengine2d::vector_utils::convertCartToIsoPoint(cartSpritePosition + gengine2d::Vector2f(TILE_SIZE, 0.f));
          fogVertex[2].position = gengine2d::vector_utils::convertCartToIsoPoint(cartSpritePosition + gengine2d::Vector2f(0.f, TILE_SIZE));
          fogVertex[3].position = fogVertex[2].position;
          fogVertex[4].position = gengine2d::vector_utils::convertCartToIsoPoint(cartSpritePosition + gengine2d::Vector2f(TILE_SIZE, TILE_SIZE));
          fogVertex[5].position = fogVertex[1].position;
          fogVertices.insert(fogVertices.end(),
                             fogVertex.begin(),
                             fogVertex.end());
        }
      }
    }

    renderer.draw2DShape(fogVertices.data(), fogVertices.size(), nullptr);
  }

  void Fog::expose(float x, float y)
  {
    auto position = std::make_pair(floorf(x), floorf(y));
    auto findIt = m_fog.find(position);
    if(findIt != m_fog.end()) {
      findIt->second = 0;
    }
  }

  void Fog::exposeRadius(float x, float y, float radius)
  {
    x = floorf(x);
    y = floorf(y);

    auto minX = x - radius;
    auto minY = y - radius;
    auto maxX = x + radius;
    auto maxY = y + radius;
    for(float spx = minX; spx <= maxX; spx += 1.f) {
      for(float spy = minY; spy <= maxY; spy += 1.f) {
        expose(spx, spy);
      }
    }
  }

  bool Fog::isExposed(float x, float y) const
  {
    auto position = std::make_pair(floorf(x), floorf(y));
    auto findIt = m_fog.find(position);
    return findIt == m_fog.end() ? false : findIt->second == 0;
  }
}
