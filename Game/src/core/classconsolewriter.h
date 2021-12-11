#ifndef CLASSCONSOLEWRITER_H
#define CLASSCONSOLEWRITER_H

#include "type.h"

namespace gengine2d
{
  class ClassConsoleWriter
  {
  public:
    explicit ClassConsoleWriter(gengine2d::string_t name);

    void print(gengine2d::string_t msg);
    void printLine(gengine2d::string_t msg);
    void printError(gengine2d::string_t msg);
    void printWarning(gengine2d::string_t msg);

  private:
    gengine2d::string_t m_name;
  };
}

#endif // CLASSCONSOLEWRITER_H
