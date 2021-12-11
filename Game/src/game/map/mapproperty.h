#ifndef MAPPROPERTY_H
#define MAPPROPERTY_H

#include "../../core/classconsolewriter.h"
#include "../../core/type.h"
#include "tileinfo.h"

namespace strat
{
  class MapProperty : gengine2d::ClassConsoleWriter
  {
  public:
    MapProperty();

    void createNew(unsigned int width, unsigned int height);

    bool saveToFile(const gengine2d::string_t& filename);
    bool saveToStream(std::ostream& stream);

    bool loadFromFile(const gengine2d::string_t& filename);
    bool loadFromStream(std::istream& stream);

    void setTileInfo(unsigned int x, unsigned int y, const map_detail::TileInfo& tileInfo);
    void setTileInfo(std::size_t index, const map_detail::TileInfo& tileInfo);
    const map_detail::TileInfo& getTileInfo(unsigned int x, unsigned int y) const;
    const map_detail::TileInfo& getTileInfo(std::size_t index) const;

    unsigned int getWidth() const { return m_width; }
    unsigned int getHeight() const { return m_height; }

  private:
    gengine2d::vector_t<map_detail::TileInfo>   m_mapData;
    unsigned int                                m_width;
    unsigned int                                m_height;
  };
}

#endif // MAPPROPERTY_H
