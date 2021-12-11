#include "map_common.h"

#include "../game_def.h"

namespace strat
{
namespace map_common
{
  gengine2d::Vector2f getTileStartPosition(float x, float y)
  {
    return gengine2d::Vector2f(x * TILE_SIZE, y * TILE_SIZE);
  }

  gengine2d::Vector2f getMinimapTileStartPosition(float x,
                                                  float y,
                                                  float miniMapWidth,
                                                  float miniMapHeight)
  {
    return gengine2d::Vector2f((x * miniMapWidth), (y * miniMapHeight));
  }

  std::pair<float, float> getMinimapRatios(float mapWidth,
                                           float mapHeight,
                                           float miniMapWidth,
                                           float miniMapHeight)
  {
    return std::make_pair(miniMapWidth / (mapWidth * TILE_SIZE),
                          miniMapHeight / (mapHeight * TILE_SIZE));
  }
}
}
