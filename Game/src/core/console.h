#ifndef CONSOLE_H
#define CONSOLE_H

//#include "game_internal_types.h"
#include <SFML/Graphics.hpp>
#include "utility/command_utility.h"
#include "inputevent.h"
#include "resourcelookup.h"
#include "type.h"

namespace gengine2d
{
  template<class T> class LockedResourceLookup;

  class Console
  {
    explicit Console(std::size_t maxMessages = 30,
                     std::size_t maxCmdHistory = 10);

    void draw();
    void handleInput(const InputEvent&);

    void clear();

    friend class Game;
    friend const fast_unordered_map_t<string_t, std::function<void(const string_t&, const vector_t<string_t>&)>>& command_utils::getCommandList();

  public:
    void print(string_t msg);
    void printLine(string_t msg);
    void printWarning(string_t msg);
    void printError(string_t msg);

  private:
    vector_t<string_t>                        m_messages;
    std::size_t                               m_maxMessages;

    vector_t<string_t>                        m_cmdHistory;
    vector_t<string_t>::iterator              m_cmdHistoryPointer;
    std::size_t                               m_maxCmdHistory;
  };
}

#endif // CONSOLE_H
