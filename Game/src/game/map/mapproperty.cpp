#include "mapproperty.h"

#include <fstream>
#include <jsonxx/jsonxx.h>
#include "../../core/debug/debug_log.h"
#include "../../core/utility/converter_utility.h"
#include "../../core/utility/general_utility.h"
#include "../utility/json_utility.h"
#include "../game_def.h"

namespace strat
{
  static constexpr auto MAP_FIELD_WIDTH = "width";
  static constexpr auto MAP_FIELD_HEIGHT = "height";
  static constexpr auto MAP_FIELD_TILE_POS = "position";
  static constexpr auto MAP_FIELD_TILE_DATA = "data";
  static constexpr auto MAP_FIELD_TILE_WALKABLE = "is_walkable";
  static constexpr auto MAP_FIELD_TILE_BUILDABLE = "is_builable";
  static constexpr auto MAP_FIELD_TILE_TEXTURE = "texture_position";

  static constexpr int MAP_SIZE_LIMIT = 200;

  MapProperty::MapProperty() :
    gengine2d::ClassConsoleWriter("MAPPropertyLoader")
  {
  }

  void MapProperty::createNew(unsigned int width, unsigned int height)
  {
    m_width = width;
    m_height = height;
    auto size = m_width * m_height;

    m_mapData.clear();
    m_mapData.resize(size);
  }

  bool MapProperty::saveToFile(const gengine2d::string_t& filename)
  {
    std::ofstream out(filename);
    if(out.is_open()) {
      return saveToStream(out);
    }

    return false;
  }

  bool MapProperty::saveToStream(std::ostream& stream)
  {
    json_utils::outputStartObject(stream);
    json_utils::outputNumber(stream, MAP_FIELD_WIDTH, m_width); json_utils::outputComma(stream);
    json_utils::outputNumber(stream, MAP_FIELD_HEIGHT, m_height); json_utils::outputComma(stream);

    int x = 0, y = 0;
    json_utils::outputArray(stream, MAP_FIELD_TILE_DATA, m_mapData.begin(), m_mapData.end(),
    [this, &stream, &x, &y](const map_detail::TileInfo& tileInfo){
      json_utils::outputStartObject(stream);
      json_utils::outputVector2(stream, MAP_FIELD_TILE_POS, gengine2d::Vector2<double>(x, y));
      ++x;
      if(x >= m_width) {
        x = 0;
        ++y;
      }
      json_utils::outputComma(stream);
      json_utils::outputBoolean(stream, MAP_FIELD_TILE_WALKABLE, tileInfo.tileFlags.walkable);
      json_utils::outputComma(stream);
      json_utils::outputBoolean(stream, MAP_FIELD_TILE_BUILDABLE, tileInfo.tileFlags.buildable);
      json_utils::outputComma(stream);

      auto textureRect = tileInfo.tileTextureRect;
      json_utils::outputRect(stream, MAP_FIELD_TILE_TEXTURE, gengine2d::Rect<double>(textureRect.left,
                                                                                     textureRect.top,
                                                                                     textureRect.width,
                                                                                     textureRect.height));
      json_utils::outputEndObject(stream);
    });
    json_utils::outputEndObject(stream);

    return true;
  }

  bool MapProperty::loadFromFile(const gengine2d::string_t& filename)
  {
    std::ifstream in(filename);
    if(in.is_open()) {
      return loadFromStream(in);
    }

    return false;
  }

  bool MapProperty::loadFromStream(std::istream& stream)
  {
    m_mapData.clear();

    jsonxx::Value v;
    if(v.parse(stream) && v.is<jsonxx::Object>()) {
      const auto& mapObject = v.get<jsonxx::Object>();
      if(mapObject.has<jsonxx::Number>(MAP_FIELD_WIDTH) &&
         mapObject.has<jsonxx::Number>(MAP_FIELD_HEIGHT)) {
        auto width = mapObject.get<jsonxx::Number>(MAP_FIELD_WIDTH);
        auto height = mapObject.get<jsonxx::Number>(MAP_FIELD_HEIGHT);

        if(gengine2d::general_utils::is_within_ex<int>(width, 0, MAP_SIZE_LIMIT) &&
           gengine2d::general_utils::is_within_ex<int>(height, 0, MAP_SIZE_LIMIT)) {
          m_width = width;
          m_height = height;
          m_mapData.resize(m_width * m_height);

          const auto& tileInfoArray = mapObject.get<jsonxx::Array>(MAP_FIELD_TILE_DATA);
          for(std::size_t i = 0, isize = tileInfoArray.size(); i < isize; ++i) {
            if(!tileInfoArray.has<jsonxx::Object>(i)) {
              DEBUG_LOG_ERR("Invalid object type");
              return false;
            }

            const auto& tileInfo = tileInfoArray.get<jsonxx::Object>(i);
            if(!json_utils::hasVector2(tileInfo, MAP_FIELD_TILE_POS) ||
               !tileInfo.has<jsonxx::Boolean>(MAP_FIELD_TILE_WALKABLE) ||
               !tileInfo.has<jsonxx::Boolean>(MAP_FIELD_TILE_BUILDABLE) ||
               !json_utils::hasRect(tileInfo, MAP_FIELD_TILE_TEXTURE)) {
              DEBUG_LOG_ERR("Missing fields");
               return false;
            }

            auto tilePosition = json_utils::getVector2<int>(tileInfo, MAP_FIELD_TILE_POS);
            if(!gengine2d::general_utils::is_within_lin<int>(tilePosition.x, 0, m_width) ||
               !gengine2d::general_utils::is_within_lin<int>(tilePosition.y, 0, m_height)) {
              DEBUG_LOG_ERR("Invalid Position");
              return false;
            }

            auto walkable = tileInfo.get<jsonxx::Boolean>(MAP_FIELD_TILE_WALKABLE);
            auto buildable = tileInfo.get<jsonxx::Boolean>(MAP_FIELD_TILE_BUILDABLE);
            auto textureRect = json_utils::getRect<int>(tileInfo, MAP_FIELD_TILE_TEXTURE);

            auto& tile = m_mapData[gengine2d::converter_utils::to1D<int>(tilePosition.x, tilePosition.y, m_width)];
            tile.tileFlags.walkable = walkable;
            tile.tileFlags.buildable = buildable;
            tile.tileTextureRect = gengine2d::Rectf(textureRect.left, textureRect.top,
                                                    textureRect.width, textureRect.height);
          }

          return true;
        }

        DEBUG_LOG_ERR("Map properties overflow");
        return false;
      }

      DEBUG_LOG_ERR("Missing map properties");
      return false;
    }

    DEBUG_LOG_ERR("Could not parse map properties");
    return false;
  }

  void MapProperty::setTileInfo(unsigned int x, unsigned int y, const map_detail::TileInfo& tileInfo)
  {
    setTileInfo(gengine2d::converter_utils::to1D(x, y, m_width), tileInfo);
  }

  void MapProperty::setTileInfo(std::size_t index, const map_detail::TileInfo& tileInfo)
  {
    assert(index < m_mapData.size());
    m_mapData[index] = tileInfo;
  }

  const map_detail::TileInfo& MapProperty::getTileInfo(unsigned int x, unsigned int y) const
  {
    return getTileInfo(gengine2d::converter_utils::to1D(x, y, m_width));
  }

  const map_detail::TileInfo& MapProperty::getTileInfo(std::size_t index) const
  {
    assert(index < m_mapData.size());
    return m_mapData[index];
  }
}
