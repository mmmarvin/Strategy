#ifndef MAP_EDITOR_COMMON_H
#define MAP_EDITOR_COMMON_H

#include "../Game/src/core/type.h"

namespace strat_me
{
  gengine2d::string_t getMapEditorName();
  gengine2d::string_t getMapEditorVersion();
  gengine2d::string_t getMapEditorCopyrightYears();
  gengine2d::string_t getMapEditorTitle(const gengine2d::string_t& mapFilename);
}

#endif // MAP_EDITOR_COMMON_H
