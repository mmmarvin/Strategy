#ifndef COMMAND_COMMANDS_H
#define COMMAND_COMMANDS_H

#include <functional>
#include "../type.h"

namespace gengine2d
{
namespace command_utils
{
  const fast_unordered_map_t<string_t, std::function<void(const string_t&,
                                                           const vector_t<string_t>&)>>& getCommandList();
}
}

#endif // COMMAND_COMMANDS_H
