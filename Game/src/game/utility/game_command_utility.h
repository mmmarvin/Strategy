#ifndef GAME_COMMAND_UTILITY_H
#define GAME_COMMAND_UTILITY_H

#include <functional>
#include "../../core/type.h"

namespace strat
{
  class InGameState;
namespace command_utils
{
  const gengine2d::fast_unordered_map_t<gengine2d::string_t, std::function<bool(const gengine2d::string_t&, const gengine2d::vector_t<gengine2d::string_t>&, InGameState&)>>& getCommandList();
}
}

#endif // GAME_COMMAND_UTILITY_H
