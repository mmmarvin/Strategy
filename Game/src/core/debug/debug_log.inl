#include "debug_log.h"

#include "std.h"

namespace gengine
{
  template<class... Args>
  void debug_log(Args&&... args)
  {
    (std_out() << ... << std::forward<Args>(args)) << std::endl;
  }

  template<class... Args>
  void debug_log_err(Args&&... args)
  {
    (std_err() << ... << std::forward<Args>(args)) << std::endl;
  }
}
