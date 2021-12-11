#include "variable_common.h"

#include "../console_err_tags.h"
#include "../game.h"

namespace gengine2d
{
namespace variable_common
{
  bool hasRequiredNumberOfTokens(std::size_t numberOfTokens, std::size_t required, const string_t& name)
  {
    auto& console = GameEngine::getInstance().getConsole();
    if(numberOfTokens == required) {
      return true;
    }

    console.printError(CETAG_CONSOLE + "Invalid use of \"" + name + "\"");
    return false;
  }

  gengine2d::VariableManager::ESetReturn setVariable(gengine2d::VariableManager& variableManager,
                                                     const gengine2d::string_t& key,
                                                     const gengine2d::string_t& value)
  {
    auto& console = GameEngine::getInstance().getConsole();

    auto setResult = variableManager.setVariable(key, value);
    switch(setResult) {
    case gengine2d::VariableManager::ESetReturn::Success:
      console.printLine("Set \"" + key + "\" to \"" + value + "\"");
      break;
    case gengine2d::VariableManager::ESetReturn::InvalidType:
      console.printLine("Cannot set \"" +
                        key +
                        "\" to \"" +
                        value +
                        "\": Types don't match");
      break;
    case gengine2d::VariableManager::ESetReturn::IsReadOnly:
      console.printLine("Cannot set \"" +
                        key +
                        "\" to \"" +
                        value +
                        "\": Variable is read-only");
      break;
    default:
      break;
    }

    return setResult;
  }
}
}
