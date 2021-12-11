#include "tileinfo.h"

#include "../game_def.h"

namespace strat
{
namespace map_detail
{
  TileFlags::TileFlags() :
    walkable(true),
    buildable(true)
  {
  }

  TileInfo::TileInfo() :
    tileTextureRect(0, 0, TILE_SIZE, TILE_SIZE)
  {
  }
}
}
