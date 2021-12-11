#include "../Game/src/game/game_config.h"
#include "map_editor_common.h"
#include "map_editor_config.h"

namespace strat_me
{
  gengine2d::string_t getMapEditorName()
  {
    return strat::GAME_TITLE +
           " " +
           "Map Editor";
  }

  gengine2d::string_t getMapEditorVersion()
  {
    return "v." +
           MAP_EDITOR_VERSION_MAJOR +
           "." +
           MAP_EDITOR_VERSION_MINOR +
           MAP_EDITOR_VERSION_PATCH;
  }

  gengine2d::string_t getMapEditorCopyrightYears()
  {
    return "2019-2020";
  }

  gengine2d::string_t getMapEditorTitle(const gengine2d::string_t& mapFilename)
  {
   return getMapEditorName() +
          " " +
          getMapEditorVersion() +
          " [" +
          mapFilename +
          "]";
  }
}
