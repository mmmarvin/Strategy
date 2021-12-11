#ifndef TILEINFO_H
#define TILEINFO_H

#include "../../core/rect.h"

namespace strat
{
namespace map_detail
{
  struct TileFlags
  {
    TileFlags();

    bool walkable;
    bool buildable;
  };

  struct TileInfo
  {
    TileInfo();

    TileFlags         tileFlags;
    gengine2d::Rectf  tileTextureRect;
  };
}
}

#endif // TILEINFO_H
