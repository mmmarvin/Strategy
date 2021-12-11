#include "classconsolewriter.h"

#include "common/console_message_common.h"
#include "game.h"

namespace gengine2d
{
  ClassConsoleWriter::ClassConsoleWriter(gengine2d::string_t name) :
    m_name(std::move(name))
  {
  }

  void ClassConsoleWriter::print(gengine2d::string_t msg)
  {
    assert(gengine2d::GameEngine::getInstance().isInitialized());
    gengine2d::GameEngine::getInstance().getConsole().print(console_message_common::generateMessage(m_name,
                                                                                                    std::move(msg)));
  }

  void ClassConsoleWriter::printLine(gengine2d::string_t msg)
  {
    assert(gengine2d::GameEngine::getInstance().isInitialized());
    gengine2d::GameEngine::getInstance().getConsole().printLine(console_message_common::generateMessage(m_name,
                                                                                                        std::move(msg)));
  }

  void ClassConsoleWriter::printError(gengine2d::string_t msg)
  {
    assert(gengine2d::GameEngine::getInstance().isInitialized());
    gengine2d::GameEngine::getInstance().getConsole().printError(console_message_common::generateMessage(m_name,
                                                                                                         std::move(msg)));
  }

  void ClassConsoleWriter::printWarning(gengine2d::string_t msg)
  {
    assert(gengine2d::GameEngine::getInstance().isInitialized());
    gengine2d::GameEngine::getInstance().getConsole().printWarning(console_message_common::generateMessage(m_name,
                                                                                                           std::move(msg)));
  }
}
