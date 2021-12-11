#include "command_utility.h"

#include "../common/variable_common.h"
#include "../config.h"
#include "../game.h"
#include "string_utility.h"

namespace gengine2d
{
namespace command_utils
{
  const fast_unordered_map_t<string_t, std::function<void(const string_t&, const vector_t<string_t>&)>>& getCommandList()
  {
    static constexpr auto clearConsoleCommand = [](const string_t& token,
                                                   const vector_t<string_t>& tokens)
    {
      if(variable_common::hasRequiredNumberOfTokens(tokens.size(), 1, token)) {
        GameEngine::getInstance().getConsole().clear();
      }
    };

    static constexpr auto quitCommand = [](const string_t& token,
                                           const vector_t<string_t>& tokens)
    {
      if(variable_common::hasRequiredNumberOfTokens(tokens.size(), 1, token)) {
        GameEngine::getInstance().quit();
      }
    };

    static constexpr auto printEngineVersionCommand = [](const string_t& token,
                                                         const vector_t<string_t>& tokens)
    {
      if(variable_common::hasRequiredNumberOfTokens(tokens.size(), 1, token)) {
        GameEngine::getInstance().getConsole().printLine("v." +
                                                         config_utils::versionToString(gengine2d_VERSION_MAJOR,
                                                                                       gengine2d_VERSION_MINOR,
                                                                                       gengine2d_VERSION_PATCH));
      }
    };

    static constexpr auto printEngineNameCommand = [](const string_t& token,
                                                      const vector_t<string_t>& tokens)
    {
      if(variable_common::hasRequiredNumberOfTokens(tokens.size(), 1, token)) {
        GameEngine::getInstance().getConsole().printLine(gengine2d_TITLE);
      }
    };

    static constexpr auto printInfoCommand = [](const string_t& token,
                                                const vector_t<string_t>& tokens)
    {
      if(variable_common::hasRequiredNumberOfTokens(tokens.size(), 1, token)) {
        GameEngine::getInstance().printPCInfo();
      }
    };

    static constexpr auto printEchoCommand = [](const string_t&,
                                                const vector_t<string_t>& tokens)
    {
      if(tokens.size() > 1) {
        GameEngine::getInstance().getConsole().printLine(combineTokens(tokens, ' ', 1, tokens.size()));
      }
    };

    static constexpr auto listCommands = [](const string_t&,
                                            const vector_t<string_t>&)
    {
      auto list = GameEngine::getInstance().getCommandList();
      std::sort(list.begin(), list.end());

      auto& console = GameEngine::getInstance().getConsole();
      console.printLine("Command List:");
      for(const auto& cmd : list) {
        console.printLine(" -" + cmd);
      }
    };

    static const fast_unordered_map_t<string_t, std::function<void(const string_t&, const vector_t<string_t>&)>> COMMAND_LIST =
    {
      { "clear", clearConsoleCommand },
      { "exit", quitCommand },
      { "quit", quitCommand },
      { "q", quitCommand },
      { "engine.version", printEngineVersionCommand},
      { "engine.name", printEngineNameCommand },
      { "info", printInfoCommand },
      { "echo", printEchoCommand},
      { "listcommands", listCommands }
    };

    return COMMAND_LIST;
  }
}
}
