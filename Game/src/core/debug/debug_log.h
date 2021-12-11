#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#ifndef NDEBUG
#include <iostream>
#include "../type.h"

namespace gengine
{
  template<class... Args>
  void debug_log(Args&&... args);

  template<class... Args>
  void debug_log_err(Args&&... args);
}

#define DEBUG_LOG(...) ::gengine::debug_log(__VA_ARGS__)
#define DEBUG_LOG_ERR(...) ::gengine::debug_log_err(__VA_ARGS__)
#define DEBUG_DEV_LOG(...) ::gengine::debug_log("[DEV LOG:]", __VA_ARGS__)
#define DEBUG_DEV_WARNING_LOG(...) ::gengine::debug_log("[DEV WARNING:]", __VA_ARGS__)

#include "debug_log.inl"

#else

#define DEBUG_LOG(...)
#define DEBUG_LOG_ERR(...)
#define DEBUG_DEV_LOG(...)
#define DEBUG_DEV_WARNING_LOG(...)

#endif // NDEBUG
#endif // DEBUG_LOG_H
