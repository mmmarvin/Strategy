#include "core/game.h"
#include "game/game_config.h"
#include "game/ingameloadinggamestate.h"
#include "game/ingamestate.h"

int main(int argc, char** argv)
{
  bool canGoConsole = false;
  for(int i = 1; i < argc; ++i) {
    gengine2d::string_t argvs(argv[i]);
    if(argvs == "--console") {
      canGoConsole = true;
    }
  }

  auto& gameEngine = gengine2d::GameEngine::getInstance();
  if(!gameEngine.init(strat::GAME_TITLE + " " +
                      strat::GAME_VERSION_STRING + " (" +
                      strat::GAME_RELEASE_TYPE + ") " +
                      strat::GAME_REVISION_STRING,
                      canGoConsole,
                      std::initializer_list<gengine2d::string_t>{},
                      std::initializer_list<std::pair<gengine2d::string_t, gengine2d::string_t>>{})) {
    gengine2d::GameLogger::getInstance().writeError("There was an error initializing gengine2d.");
    return -1;
  }

  gameEngine.addState<strat::InGameLoadingGameState>(std::make_unique<strat::InGameState>());
  gameEngine.run();

  return 0;
}
