#ifndef MAP_COMMON_H
#define MAP_COMMON_H

#include "../../core/vector2.h"

namespace strat
{
namespace map_common
{
  gengine2d::Vector2f getTileStartPosition(float x, float y);
  gengine2d::Vector2f getMinimapTileStartPosition(float x,
                                                  float y,
                                                  float minimapWidth,
                                                  float minimapHeight);
  std::pair<float, float> getMinimapRatios(float mapWidth,
                                           float mapHeight,
                                           float miniMapWidth,
                                           float miniMapHeight);
}
}

#endif // MAP_COMMON_H
