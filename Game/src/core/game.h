#ifndef GAME_H
#define GAME_H

#include <memory>
#include <SFML/Graphics.hpp>
#include "3rdparty/time.h"
#include "debug/debug_log.h"
#include "styles/text2dstyle.h"
#include "utility/command_utility.h"
#include "game_internal_types.h"
#include "gamestate.h"
#include "console.h"
#include "logger.h"
#include "resourcelookup.h"
#include "singleton.h"
#include "variablemanager.h"

namespace gengine2d
{
  class LoadingGameState;
  class Game
  {
    struct FontDeleter
    {
      void operator()(game_internal::Renderer::Font& font);
    };

    struct TextureDeleter
    {
      void operator()(game_internal::Renderer::Texture&) {}
    };

    template<class T,
             class DeleterT = detail::ResourceLookupDefaultDeleter<T>>
    class LockedResourceLookup
    {
    public:
      LockedResourceLookup(string_t lookupTag);

      bool exists(const string_t& name) const { return m_resourceLookup.exists(name); }
      std::size_t size() const { return m_resourceLookup.size(); }

      const T& getResource(const string_t& name) const;

    private:
      friend class Game;
      friend class GameState;

      string_t                    m_lookupTag;
      ResourceLookup<T, DeleterT> m_resourceLookup;
    };

    class ResourceManager
    {
    public:
      ResourceManager();

      LockedResourceLookup<game_internal::Renderer::Font, FontDeleter>& getFontLookup() { return m_fontLookup; }
      LockedResourceLookup<game_internal::Renderer::Texture, TextureDeleter>& getTextureLookup() { return m_textureLookup; }

//      LockedResourceLookup<sf::Sound>& getSoundLookup() { return m_soundLookup; }
//      LockedResourceLookup<sf::Music>& getMusicLookup() { return m_musicLookup; }

    private:
      LockedResourceLookup<game_internal::Renderer::Font, FontDeleter>        m_fontLookup;
      LockedResourceLookup<game_internal::Renderer::Texture, TextureDeleter>  m_textureLookup;

//      LockedResourceLookup<sf::Sound> m_soundLookup;
//      LockedResourceLookup<sf::Music> m_musicLookup;
    };

    Game();
    ~Game();

    friend class Console;
    friend class Singleton<Game>;
    friend const fast_unordered_map_t<string_t, std::function<void(const string_t&, const vector_t<string_t>&)>>& command_utils::getCommandList();

  public:
    bool init(string_t title,
              bool canGoConsole,
              std::initializer_list<string_t> convertToReadOnly,
              std::initializer_list<std::pair<string_t, string_t>> defaultValues);

    template<class StateT, class... Args> void addState(Args&&... args);
    void addStateInstance(std::unique_ptr<GameState> state);
    void addStateInstanceInitialzied(std::unique_ptr<GameState> state);
    void popState();

    void apply(string_t cmd);

    void run();
    void quit();

    game_internal::Renderer& getRenderer() { return m_renderer; }
    game_internal::InputHandler& getInputHandler() { return m_inputHandler; }
//    game_internal_type::AudioManager& getAudioManager() { return m_audioManager; }

    Console& getConsole() { return m_console; }

    ResourceManager& getResourceManager() { return m_resourceManager; }

    bool isOnConsoleMode() const { return m_consoleMode; }
    bool isInitialized() const { return m_initialized; }

    void setPrevStateMessage(string_t msg);

  private:
    friend class LoadingGameState;

    enum class EStateAction : unsigned char
    {
      Push,
      PushInitialized,
      Pop,
      Quit
    };

    vector_t<string_t> getCommandList() const;

    void printPCInfo();
    string_t getPCInfo();

    void draw(float alpha, unsigned int fps);
    void update(float dt);
    void interpolate(float dt, float accumulator);
    void handleInput();

    void drawMessages();

    void lockStates();
    void unlockStates();
    void updateStates();
    GameState* getCurrentState() { return m_currentState; }
    const GameState* getCurrentState() const { return m_currentState; }

    void addDefaultGameVariables();
    void changeDefaultGameVariableReadOnly(std::initializer_list<string_t> convertToReadOnly);
    void changeDefaultGameVariableValues(std::initializer_list<std::pair<string_t, string_t>> defaultValues);
    void loadVariables();

    void loadDefaultResources();

    vector_t<mar::time::microseconds::value_type>
                                          m_debugTimes;
    sf::Text                              m_debugText;
    Text2DStyle                           m_debugTextStyle;

//    game_internal_type::AudioManager::Music
//                                          m_testMusic;

    game_internal::Renderer               m_renderer;
    game_internal::InputHandler           m_inputHandler;
//    game_internal_type::AudioManager      m_audioManager;

    ResourceManager                       m_resourceManager;

    Console                               m_console;
    VariableManager                       m_variableManager;

    vector_t<std::pair<EStateAction, std::unique_ptr<GameState>>>
                                          m_statesCache;
    vector_t<std::unique_ptr<GameState>>  m_states;
    GameState*                            m_currentState;

    const bool*                           m_varFlagDebugShowFPS;
    const bool*                           m_varFlagDebugShowBasicProf;

    bool                                  m_statesLocked;

    bool                                  m_consoleMode;
    bool                                  m_canGoConsole;

    bool                                  m_initialized;
  };
namespace game_utils
{
  void loadVariables(const string_t& filename, VariableManager& variableManager);
}
  using GameEngine = Singleton<Game>;
}

#include "game.inl"

#endif // GAME_H
