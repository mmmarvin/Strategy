#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include "../../core/classconsolewriter.h"
#include "../../core/game_internal_types.h"
#include "../../core/vector2.h"
#include "../../core/type.h"
#include "mapproperty.h"
#include "tileinfo.h"

namespace strat
{
  class Map : gengine2d::ClassConsoleWriter
  {
  public:
    Map();

    bool loadFromFile(const gengine2d::string_t& filename,
                      float miniMapWidth,
                      float miniMapHeight);

    void draw() const;
    void drawMiniMap() const;

    void setMinimapToIsometric();
    void setMinimapToCartesian();

    const map_detail::TileFlags* getTileInfo(int x, int y) const;
    gengine2d::Vector2<std::size_t> getSize() const;

  private:
    std::array<gengine2d::Vertex2, 6> createTileOnPosition(int x, int y);
    std::array<gengine2d::Vertex2, 6> createMiniMapTileOnPosition(float miniMapTileWidth,
                                                                  float miniMapTileHeight,
                                                                  float x,
                                                                  float y);

    MapProperty                                     m_property;

    gengine2d::vector_t<gengine2d::Vertex2>         m_vertices;
    gengine2d::vector_t<gengine2d::Vertex2>         m_lineVertices;
    gengine2d::vector_t<gengine2d::Vertex2>         m_miniMapVertices;

    gengine2d::game_internal::Renderer::Texture     m_texture;
  };
}

#endif // MAP_H
