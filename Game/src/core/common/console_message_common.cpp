#include "console_message_common.h"

namespace gengine2d
{
namespace console_message_common
{
  string_t generateMessage(string_t tag, string_t msg)
  {
    return "[" + std::move(tag) + "]: " + std::move(msg);
  }
}
}
