#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "../core/utility/stl_utility.h"
#include "../core/type.h"

namespace strat
{
  static const unsigned int         GAME_VERSION_MAJOR = 0;
  static const unsigned int         GAME_VERSION_MINOR = 1;
  static const unsigned int         GAME_VERSION_PATCH = 0;
  static const unsigned int         GAME_REVISION_NUMBER = 1;

  static const gengine2d::string_t  GAME_TITLE = "Project Strategy";
  static const gengine2d::string_t  GAME_VERSION_STRING = "v." +
                                                          gengine2d::stl_utils::to_string(GAME_VERSION_MAJOR) +
                                                          "." +
                                                          gengine2d::stl_utils::to_string(GAME_VERSION_MINOR) +
                                                          gengine2d::stl_utils::to_string(GAME_VERSION_PATCH);
  static const gengine2d::string_t  GAME_RELEASE_TYPE = "Alpha";
  static const gengine2d::string_t  GAME_REVISION_STRING = "(Revision " +
                                                           gengine2d::stl_utils::to_string(GAME_REVISION_NUMBER) +
                                                           ")";
}

#endif // GAME_CONFIG_H
