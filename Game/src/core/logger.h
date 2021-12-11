#ifndef LOGGER_H
#define LOGGER_H

#include "type.h"
#include "singleton.h"

namespace gengine2d
{
  class Logger
  {
    Logger() {}

    friend class Singleton<Logger>;

  public:
    void write(string_t msg);
    void writeError(string_t msg);
    void writeWarning(string_t msg);
  };

  using GameLogger = Singleton<Logger>;
}

#endif // LOGGER_H
