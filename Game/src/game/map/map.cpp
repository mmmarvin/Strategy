#include "map.h"

#include "../../core/utility/stream_utility.h"
#include "../../core/utility/vector_utility.h"
#include "../../core/game.h"
#include "../../core/archive_type.h"
#include "../common/map_common.h"
#include "../game_def.h"

namespace strat
{
  Map::Map() :
    gengine2d::ClassConsoleWriter("MAPLoader")
  {
  }

  bool Map::loadFromFile(const gengine2d::string_t& filename,
                         float miniMapWidth,
                         float miniMapHeight)
  {
    auto& console = gengine2d::GameEngine::getInstance().getConsole();

    gengine2d::archive_t mapArchive;
    if(mapArchive.open(filename) != gengine2d::archive_t::EOR_SUCCESS) {
      this->printError("Cannot open map file \"" + filename + "\"!");
      return false;
    }

    auto* tileInfoFile = mapArchive.getFile("tileInfo.json");
    auto* tileTextureFile = mapArchive.getFile("tileTexture.png");
    if(!tileInfoFile) {
      this->printError("Missing tile data file!");
      return false;
    }
    if(!tileTextureFile) {
      this->printError("Missing tile texture file!");
    }

    if(!m_property.loadFromStream(*tileInfoFile->getInputStream())) {
      return false;
    }

    {
      auto tileTextureBin = gengine2d::stream_utils::copyStreamToArray(*tileTextureFile->getInputStream());
      auto tileTextureLoadResult = gengine2d::GameEngine::getInstance().getRenderer().loadTexture("",
                                                                                                  tileTextureBin);

      if(!tileTextureLoadResult.first) {
        this->printError("There was a problem loading the texture file!");
        return false;
      }

      m_texture = std::move(tileTextureLoadResult.second);
    }

    auto width = m_property.getWidth();
    auto height = m_property.getHeight();

    auto [xRatio, yRatio] = map_common::getMinimapRatios(width, height, miniMapWidth, miniMapHeight);
    auto miniMapTileWidth = TILE_SIZE * xRatio;
    auto miniMapTileHeight = TILE_SIZE * yRatio;

    m_vertices.clear();
    for(int x = 0; x < width; ++x) {
      for(int y = 0; y < height; ++y) {
        auto vertex = createTileOnPosition(x, y);
        m_vertices.insert(m_vertices.end(),
                          vertex.begin(),
                          vertex.end());

        vertex[0].color = gengine2d::Color::Black;
        vertex[1].color = gengine2d::Color::Black;
        vertex[2].color = gengine2d::Color::Black;
        vertex[3].color = gengine2d::Color::Black;

        m_lineVertices.push_back(vertex[0]);
        m_lineVertices.push_back(vertex[1]);

        m_lineVertices.push_back(vertex[1]);
        m_lineVertices.push_back(vertex[4]);

        m_lineVertices.push_back(vertex[4]);
        m_lineVertices.push_back(vertex[2]);

        m_lineVertices.push_back(vertex[2]);
        m_lineVertices.push_back(vertex[0]);

        auto miniMapVertex = createMiniMapTileOnPosition(miniMapTileWidth,
                                                         miniMapTileHeight,
                                                         x,
                                                         y);
        m_miniMapVertices.insert(m_miniMapVertices.end(),
                                 miniMapVertex.begin(),
                                 miniMapVertex.end());
      }
    }

    return true;
  }

  void Map::draw() const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();

    renderer.draw2DShape(m_vertices.data(),
                         m_vertices.size(),
                         &m_texture);
    renderer.draw2DLines(m_lineVertices.data(),
                         m_lineVertices.size());
  }

  void Map::drawMiniMap() const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    renderer.draw2DShape(m_miniMapVertices.data(),
                         m_miniMapVertices.size(),
                         &m_texture);
  }

  void Map::setMinimapToIsometric()
  {
    for(auto& vertex : m_miniMapVertices) {
      vertex.position = gengine2d::vector_utils::convertCartToIsoPoint(vertex.position);
    }
  }

  void Map::setMinimapToCartesian()
  {
    for(auto& vertex : m_miniMapVertices) {
      vertex.position = gengine2d::vector_utils::convertIsoToCartPoint(vertex.position);
    }
  }

  const map_detail::TileFlags* Map::getTileInfo(int x, int y) const
  {
    auto width = m_property.getWidth();
    auto height = m_property.getHeight();
    auto maxSize = width * height;

    long int pos = x * width + y;
    if(pos >= 0 && pos < maxSize) {
      return &m_property.getTileInfo(pos).tileFlags;
    }

    return nullptr;
  }

  gengine2d::Vector2<std::size_t> Map::getSize() const
  {
    return gengine2d::Vector2<std::size_t>(m_property.getWidth(), m_property.getHeight());
  }

  std::array<gengine2d::Vertex2, 6> Map::createTileOnPosition(int x, int y)
  {
    std::array<gengine2d::Vertex2, 6> vertex;
    vertex[0].position = map_common::getTileStartPosition(x, y);
    vertex[1].position = vertex[0].position + gengine2d::Vector2f(TILE_SIZE, 0.f);
    vertex[2].position = vertex[0].position + gengine2d::Vector2f(0.f, TILE_SIZE);

    vertex[3].position = vertex[2].position;
    vertex[4].position = vertex[0].position + gengine2d::Vector2f(TILE_SIZE, TILE_SIZE);
    vertex[5].position = vertex[1].position;

    vertex[0].position = gengine2d::vector_utils::convertCartToIsoPoint(vertex[0].position);
    vertex[1].position = gengine2d::vector_utils::convertCartToIsoPoint(vertex[1].position);
    vertex[2].position = gengine2d::vector_utils::convertCartToIsoPoint(vertex[2].position);
    vertex[3].position = gengine2d::vector_utils::convertCartToIsoPoint(vertex[3].position);
    vertex[4].position = gengine2d::vector_utils::convertCartToIsoPoint(vertex[4].position);
    vertex[5].position = gengine2d::vector_utils::convertCartToIsoPoint(vertex[5].position);

    const auto& tileTextureRect = m_property.getTileInfo(x, y).tileTextureRect;
    vertex[0].textureCoordinate = gengine2d::Vector2f(tileTextureRect.left, tileTextureRect.top);
    vertex[1].textureCoordinate = vertex[0].textureCoordinate + gengine2d::Vector2f(tileTextureRect.width, 0.f);
    vertex[2].textureCoordinate = vertex[0].textureCoordinate + gengine2d::Vector2f(0.f, tileTextureRect.height);

    vertex[3].textureCoordinate = vertex[2].textureCoordinate;
    vertex[4].textureCoordinate = vertex[0].textureCoordinate + gengine2d::Vector2f(tileTextureRect.width,
                                                                                    tileTextureRect.height);
    vertex[5].textureCoordinate = vertex[1].textureCoordinate;

    return vertex;
  }

  std::array<gengine2d::Vertex2, 6> Map::createMiniMapTileOnPosition(float miniMapTileWidth,
                                                                     float miniMapTileHeight,
                                                                     float x,
                                                                     float y)
  {
    std::array<gengine2d::Vertex2, 6> miniMapVertex;
    miniMapVertex[0].position = map_common::getMinimapTileStartPosition(x, y, miniMapTileWidth, miniMapTileHeight);
    miniMapVertex[1].position = miniMapVertex[0].position + gengine2d::Vector2f(miniMapTileWidth, 0.f);
    miniMapVertex[2].position = miniMapVertex[0].position + gengine2d::Vector2f(0.f, miniMapTileHeight);

    miniMapVertex[3].position = miniMapVertex[2].position;
    miniMapVertex[4].position = miniMapVertex[0].position + gengine2d::Vector2f(miniMapTileWidth,
                                                                                miniMapTileHeight);
    miniMapVertex[5].position = miniMapVertex[1].position;

    const auto& tileTextureRect = m_property.getTileInfo(x, y).tileTextureRect;
    miniMapVertex[0].textureCoordinate = gengine2d::Vector2f(tileTextureRect.left, tileTextureRect.top);
    miniMapVertex[1].textureCoordinate = miniMapVertex[0].textureCoordinate + gengine2d::Vector2f(1.f, 0.f);
    miniMapVertex[2].textureCoordinate = miniMapVertex[0].textureCoordinate + gengine2d::Vector2f(0.f, 1.f);

    miniMapVertex[3].textureCoordinate = miniMapVertex[2].textureCoordinate;
    miniMapVertex[4].textureCoordinate = miniMapVertex[0].textureCoordinate + gengine2d::Vector2f(1.f, 1.f);
    miniMapVertex[5].textureCoordinate = miniMapVertex[1].textureCoordinate;

    return miniMapVertex;
  }
}
