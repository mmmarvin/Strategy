#ifndef DEFINE_H
#define DEFINE_H

#include <cinttypes>
#include "type.h"

namespace gengine2d
{
  static constexpr std::size_t STREAM_BUFFER_SIZE = 512;
  static constexpr std::size_t THREAD_MIN_ITEM_PER_THREAD = 25;

  static constexpr auto CONFIG_FILENAME = "cfg/config.cfg";
  static constexpr auto CUSTOM_CONFIG_FILENAME = "cfg/autorun.cfg";
  static constexpr auto LOG_FILENAME = "log/game_log.log";

  static constexpr char NULL_CHAR = '\0';
}

#endif // DEFINE_H
