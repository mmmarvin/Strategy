#include "mapeditorcanvas.h"

#include <iostream>
#include <QMouseEvent>
#include <QStatusBar>
#include <QResizeEvent>

#include "../Game/src/core/debug/debug_log.h"
#include "../Game/src/core/utility/collider_utility.h"
#include "../Game/src/core/utility/converter_utility.h"
#include "../Game/src/core/utility/general_utility.h"
#include "../Game/src/core/utility/shape_utility.h"
#include "../Game/src/core/utility/stream_utility.h"
#include "../Game/src/core/utility/vector_utility.h"
#include "../Game/src/core/archive_type.h"
#include "../Game/src/game/common/map_common.h"
#include "../Game/src/game/utility/game_stream_utility.h"
#include "../Game/src/game/game_def.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rdparty/stb/stb_image_write.h"

namespace strat_me
{
namespace
{
  std::size_t positionFrom2DTo1D(int x, int y, int width)
  {
    return (y * width) + x;
  }

  void copyToStream(std::ostream& stream, const sf::Texture& texture)
  {
    auto image = texture.copyToImage();

    int pngLen = 0;
    auto* pngData = stbi_write_png_to_mem(image.getPixelsPtr(), 0, image.getSize().x, image.getSize().y, 4, &pngLen);
    if(pngData) {
      stream.write(reinterpret_cast<const char*>(pngData), pngLen);
    }
  }
}
  MapEditorCanvas::MapEditorCanvas(QWidget* parent, const QSize& size) :
    QSFMLCanvas(parent, size, 64),
    m_currentTileSelected(0, 0, 0, 0),
    m_mode(EMode::MODE_SELECT),
    m_dragStarted(false),
    m_showGridLine(true),
    m_isIsometric(true)
  {
  }

  void MapEditorCanvas::createNew(unsigned int width, unsigned int height)
  {
    m_mapProperty.createNew(width, height);

    for(unsigned int j = 0; j < width; ++j) {
      for(unsigned int i = 0; i < height; ++i) {
        strat::map_detail::TileInfo tileInfo;
        tileInfo.tileFlags.buildable = true;
        tileInfo.tileFlags.buildable = true;
        tileInfo.tileTextureRect = sf::FloatRect(0.f, 0.f, strat::TILE_SIZE, strat::TILE_SIZE);

        std::array<sf::Vertex, 4> tiles;
        tiles[0].position = strat::map_common::getTileStartPosition(i, j);
        tiles[1].position = tiles[0].position + gengine2d::Vector2f(strat::TILE_SIZE, 0.f);
        tiles[2].position = tiles[0].position + gengine2d::Vector2f(strat::TILE_SIZE, strat::TILE_SIZE);
        tiles[3].position = tiles[0].position + gengine2d::Vector2f(0.f, strat::TILE_SIZE);

        tiles[0].position = gengine2d::vector_utils::convertCartToIsoPoint(tiles[0].position);
        tiles[1].position = gengine2d::vector_utils::convertCartToIsoPoint(tiles[1].position);
        tiles[2].position = gengine2d::vector_utils::convertCartToIsoPoint(tiles[2].position);
        tiles[3].position = gengine2d::vector_utils::convertCartToIsoPoint(tiles[3].position);

        tiles[0].texCoords = gengine2d::Vector2f(tileInfo.tileTextureRect.left, tileInfo.tileTextureRect.top);
        tiles[1].texCoords = gengine2d::Vector2f(tileInfo.tileTextureRect.left + tileInfo.tileTextureRect.width, tileInfo.tileTextureRect.top);
        tiles[2].texCoords = gengine2d::Vector2f(tileInfo.tileTextureRect.left + tileInfo.tileTextureRect.width, tileInfo.tileTextureRect.top + tileInfo.tileTextureRect.height);
        tiles[3].texCoords = gengine2d::Vector2f(tileInfo.tileTextureRect.left, tileInfo.tileTextureRect.top + tileInfo.tileTextureRect.height);

        std::array<sf::Vertex, 8> tileLines;
        tileLines[0].position = strat::map_common::getTileStartPosition(i, j);
        tileLines[1].position = tileLines[0].position + gengine2d::Vector2f(strat::TILE_SIZE, 0.f);

        tileLines[2].position = tileLines[0].position + gengine2d::Vector2f(strat::TILE_SIZE, 0.f);
        tileLines[3].position = tileLines[0].position + gengine2d::Vector2f(strat::TILE_SIZE, strat::TILE_SIZE);

        tileLines[4].position = tileLines[0].position + gengine2d::Vector2f(strat::TILE_SIZE, strat::TILE_SIZE);
        tileLines[5].position = tileLines[0].position + gengine2d::Vector2f(0.f, strat::TILE_SIZE);

        tileLines[6].position = tileLines[0].position + gengine2d::Vector2f(0.f, strat::TILE_SIZE);
        tileLines[7].position = tileLines[0].position;

        tileLines[0].position = gengine2d::vector_utils::convertCartToIsoPoint(tileLines[0].position);
        tileLines[1].position = gengine2d::vector_utils::convertCartToIsoPoint(tileLines[1].position);
        tileLines[2].position = gengine2d::vector_utils::convertCartToIsoPoint(tileLines[2].position);
        tileLines[3].position = gengine2d::vector_utils::convertCartToIsoPoint(tileLines[3].position);
        tileLines[4].position = gengine2d::vector_utils::convertCartToIsoPoint(tileLines[4].position);
        tileLines[5].position = gengine2d::vector_utils::convertCartToIsoPoint(tileLines[5].position);
        tileLines[6].position = gengine2d::vector_utils::convertCartToIsoPoint(tileLines[6].position);
        tileLines[7].position = gengine2d::vector_utils::convertCartToIsoPoint(tileLines[7].position);

        tileLines[0].color = sf::Color::Black;
        tileLines[1].color = sf::Color::Black;
        tileLines[2].color = sf::Color::Black;
        tileLines[3].color = sf::Color::Black;
        tileLines[4].color = sf::Color::Black;
        tileLines[5].color = sf::Color::Black;
        tileLines[6].color = sf::Color::Black;
        tileLines[7].color = sf::Color::Black;

        m_tiles.push_back(std::make_pair(tiles, tileLines));
        m_mapProperty.setTileInfo(i, j, tileInfo);
      }
    }

    m_tileTexture.loadFromFile("tile.png");
  }

  bool MapEditorCanvas::loadFromFile(const gengine2d::string_t& filename)
  {
    gengine2d::archive_t archive;
    if(archive.open(filename) == gengine2d::archive_t::EOR_SUCCESS) {
      auto tilesInfoStream = archive.getFile("tileInfo.json");
      auto tileTextureStream = archive.getFile("tileTexture.png");

      if(!m_mapProperty.loadFromStream(*tilesInfoStream->getInputStream())) {
        return false;
      }

      auto tileTextureData = gengine2d::stream_utils::copyStreamToArray(*tileTextureStream->getInputStream());
      if(!m_tileTexture.loadFromMemory(tileTextureData.data(), tileTextureData.size())) {
        return false;
      }

      for(unsigned int y = 0; y < m_mapProperty.getHeight(); ++y) {
        for(unsigned int x = 0; x < m_mapProperty.getWidth(); ++x) {
          const auto& tileInfo = m_mapProperty.getTileInfo(x, y);
          auto& tile = m_tiles[gengine2d::converter_utils::to1D(x, y, m_mapProperty.getWidth())].first;
          tile[0].texCoords = gengine2d::Vector2f(tileInfo.tileTextureRect.left,
                                                  tileInfo.tileTextureRect.top);
          tile[1].texCoords = tile[0].texCoords + gengine2d::Vector2f(tileInfo.tileTextureRect.width, 0.f);
          tile[2].texCoords = tile[0].texCoords + gengine2d::Vector2f(tileInfo.tileTextureRect.width,
                                                                      tileInfo.tileTextureRect.height);
          tile[3].texCoords = tile[0].texCoords + gengine2d::Vector2f(0.f, tileInfo.tileTextureRect.height);
        }
      }

      return true;
    }

    return false;
  }

  bool MapEditorCanvas::saveToFile(const gengine2d::string_t& filename)
  {
    gengine2d::archive_t archive;
    std::stringstream dataStream (std::ios_base::in |
                                  std::ios_base::out |
                                  std::ios_base::binary);
    m_mapProperty.saveToStream(dataStream);

    // Save the tile info
    if(archive.addFile(dataStream, "tileInfo.json", "") != gengine2d::archive_t::EAR_SUCCESS) {
      return false;
    }

    // Save the texture info
    gengine2d::stream_utils::clearStringStreamBinary(dataStream);
    copyToStream(dataStream, m_tileTexture);
    if(archive.addFile(dataStream, "tileTexture.png", "") != gengine2d::archive_t::EAR_SUCCESS) {
      return false;
    }

    return archive.save(filename) == gengine2d::archive_t::ESR_SUCCESS;
  }

  void MapEditorCanvas::resetTileTextureSizes(float width, float height)
  {
    for(auto& [tile, tileLines] : m_tiles) {
      tile[0].texCoords = gengine2d::Vector2f(0.f, 0.f);
      tile[1].texCoords = tile[0].texCoords + gengine2d::Vector2f(width, 0.f);
      tile[2].texCoords = tile[0].texCoords + gengine2d::Vector2f(width, height);
      tile[3].texCoords = tile[0].texCoords + gengine2d::Vector2f(0.f, height);
    }
  }

  void MapEditorCanvas::setMode(EMode mode)
  {
    m_mode = mode;
  }

  bool MapEditorCanvas::setTileTexture(const gengine2d::string_t& textureFilename)
  {
    return m_tileTexture.loadFromFile(textureFilename);
  }

  void MapEditorCanvas::setTileSelectionRect(const sf::FloatRect& selectionRect)
  {
    m_currentTileSelected = selectionRect;
  }

  void MapEditorCanvas::showGridLines(bool show)
  {
    m_showGridLine = show;
  }

  void MapEditorCanvas::toggleGridLines()
  {
    m_showGridLine = !m_showGridLine;
  }

  void MapEditorCanvas::setIsometric(bool isometric)
  {
    if(isometric != m_isIsometric) {
      m_isIsometric = isometric;

      if(isometric) {
        for(auto& [tile, tileLines] : m_tiles) {
          for(auto& point : tile) {
            point.position = gengine2d::vector_utils::convertCartToIsoPoint(point.position);
          }
          for(auto& point : tileLines) {
            point.position = gengine2d::vector_utils::convertCartToIsoPoint(point.position);
          }
        }
      } else {
        for(auto& [tile, tileLines] : m_tiles) {
          for(auto& point : tile) {
            point.position = gengine2d::vector_utils::convertIsoToCartPoint(point.position);
          }
          for(auto& point : tileLines) {
            point.position = gengine2d::vector_utils::convertIsoToCartPoint(point.position);
          }
        }
      }
    }
  }

  void MapEditorCanvas::init()
  {
    createNew(100, 100);
  }

  void MapEditorCanvas::draw(sf::RenderWindow& window)
  {
    window.setView(m_view);
    auto drawableArea = gengine2d::shape_utils::getRectPoints(m_view.getCenter(), m_view.getSize());
    if(m_isIsometric) {
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
      int minX = gengine2d::general_utils::max<int>(cartRectMin.x / strat::TILE_SIZE, 0);
      int minY = gengine2d::general_utils::max<int>(cartRectMin.y / strat::TILE_SIZE, 0);
      int maxX = gengine2d::general_utils::min<int>(cartRectMax.x / strat::TILE_SIZE, m_mapProperty.getWidth());
      int maxY = gengine2d::general_utils::min<int>(cartRectMax.y / strat::TILE_SIZE, m_mapProperty.getHeight());
      for(int  y = minY; y <= maxY; ++y) {
        for(int x = minX; x <= maxX; ++x) {
          const auto& [tiles, tileLines] = m_tiles[positionFrom2DTo1D(x, y, m_mapProperty.getWidth())];
          window.draw(tiles.data(), 4, sf::PrimitiveType::Quads, sf::RenderStates(&m_tileTexture));
          if(m_showGridLine) {
            window.draw(tileLines.data(), 8, sf::PrimitiveType::Lines);
          }
        }
      }
    } else {
      auto minX = gengine2d::general_utils::max<int>(drawableArea[0] / strat::TILE_SIZE, 0);
      auto minY = gengine2d::general_utils::max<int>(drawableArea[1] / strat::TILE_SIZE, 0);
      auto maxX = gengine2d::general_utils::min<int>((minX + drawableArea[2]) / strat::TILE_SIZE, m_mapProperty.getWidth());
      auto maxY = gengine2d::general_utils::min<int>((minY + drawableArea[3]) / strat::TILE_SIZE, m_mapProperty.getHeight());

      for(int  y = minY; y <= maxY; ++y) {
        for(int x = minX; x <= maxX; ++x) {
          const auto& [tiles, tileLines] = m_tiles[positionFrom2DTo1D(x, y, m_mapProperty.getWidth())];
          window.draw(tiles.data(), 4, sf::PrimitiveType::Quads, sf::RenderStates(&m_tileTexture));
          if(m_showGridLine) {
            window.draw(tileLines.data(), 8, sf::PrimitiveType::Lines);
          }
        }
      }
    }
    window.setView(window.getDefaultView());
  }

  void MapEditorCanvas::resizeEvent(QResizeEvent* event)
  {
    QSFMLCanvas::resizeEvent(event);
    m_view.setSize(sf::Vector2f(event->size().width(), event->size().height()));
  }

  void MapEditorCanvas::mousePressEvent(QMouseEvent* event)
  {
    auto button = event->button();
    auto mousePosition = event->pos();

    if(button == Qt::MouseButton::LeftButton) {
      switch(m_mode) {
      case EMode::MODE_SELECT:
        break;
      case EMode::MODE_MOVE:
        startMove(Qt::MouseButton::LeftButton, mousePosition);
        break;
      case EMode::MODE_TILE:
        startTile();
        break;
      case EMode::MODE_OBJECT:
        break;
      default:
        break;
      }
    } else if(button == Qt::MouseButton::RightButton) {
      startMove(Qt::MouseButton::RightButton, mousePosition);
    }
  }

  void MapEditorCanvas::mouseReleaseEvent(QMouseEvent* event)
  {
    auto button = event->button();
    auto mousePosition = event->pos();

    if(m_dragStarted) {
      if(button == m_dragButton) {
        m_dragStarted = false;
      }
    }
  }

  void MapEditorCanvas::mouseMoveEvent(QMouseEvent* event)
  {
    auto mousePosition = event->pos();
    auto isoMousePosition = gengine2d::Vector2i(mousePosition.x(), mousePosition.y());
    auto cartMousePosition = gengine2d::vector_utils::convertTo<int>(gengine2d::vector_utils::convertIsoToCartPoint(gengine2d::Vector2f(mousePosition.x(), mousePosition.y())));

    if(m_dragStarted) {
      auto dragValue = m_dragStartPosition - gengine2d::Vector2f(mousePosition.x(), mousePosition.y());
      auto dragDirection = gengine2d::normalize(dragValue);
      if(dragDirection .x < m_dragDirection.x ||
         dragDirection .y < m_dragDirection.y ||
         dragDirection .x > m_dragDirection.x ||
         dragDirection .y > m_dragDirection.y) {
        m_dragDirection = dragDirection ;
        m_dragStartPosition = gengine2d::Vector2f(mousePosition.x(), mousePosition.y());
      }
      m_view.move(dragValue);
    }
    auto pos = "[" + std::to_string(isoMousePosition.x) + ", " + std::to_string(isoMousePosition.y) + "]" +
               "[" + std::to_string(cartMousePosition.x) + ", " + std::to_string(cartMousePosition.y) + "]";
    static_cast<QMainWindow*>(parent())->statusBar()->showMessage(pos.c_str());
  }

  void MapEditorCanvas::startMove(Qt::MouseButton button, const QPoint& mousePosition)
  {
    m_dragButton = button;
    m_dragStarted = true;
    m_dragStartPosition = gengine2d::Vector2f(mousePosition.x(), mousePosition.y());
  }

  void MapEditorCanvas::startTile()
  {
    if(m_currentTileSelected.width > 0.f && m_currentTileSelected.height > 0.f) {
      auto& renderWindow = this->getRenderWindow();
      auto isoMousePosition = renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow),
                                                            m_view);
      auto cartMousePosition = gengine2d::vector_utils::convertIsoToCartPoint(isoMousePosition) ;
      auto spatialPosition = gengine2d::vector_utils::convertTo<int>(cartMousePosition / strat::TILE_SIZE);
      if(spatialPosition.x >= 0 && spatialPosition.x <= m_mapProperty.getWidth() &&
         spatialPosition.y >= 0 && spatialPosition.y <= m_mapProperty.getHeight()) {
        auto arrayPosition = positionFrom2DTo1D(spatialPosition.x, spatialPosition.y, m_mapProperty.getWidth());

        auto l = m_currentTileSelected.left,
             t = m_currentTileSelected.top,
             w = m_currentTileSelected.width,
             h = m_currentTileSelected.height;


        m_tiles[arrayPosition].first[0].texCoords = gengine2d::Vector2f(l, t);
        m_tiles[arrayPosition].first[1].texCoords = gengine2d::Vector2f(l + w, t);
        m_tiles[arrayPosition].first[2].texCoords = gengine2d::Vector2f(l + w, t + h);
        m_tiles[arrayPosition].first[3].texCoords = gengine2d::Vector2f(l, t + h);

        auto tileInfo = m_mapProperty.getTileInfo(arrayPosition);
        tileInfo.tileTextureRect = m_currentTileSelected;
        m_mapProperty.setTileInfo(arrayPosition, tileInfo);
      }
    }
  }
}
