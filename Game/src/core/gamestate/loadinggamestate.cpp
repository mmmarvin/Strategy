#include "loadinggamestate.h"

#include "../game.h"

namespace gengine2d
{
  LoadingGameState::LoadingGameState(std::unique_ptr<LoadableGameState> gameStateToLoad) :
    gengine2d::GameState({}, {}),
    m_gameStateToLoad(std::move(gameStateToLoad)),
    m_startedInitialization(false)
  {
  }

  LoadingGameState::~LoadingGameState()
  {
    if(m_loadThread.joinable()) {
      m_loadThread.join();
    }
  }

  void LoadingGameState::update(float)
  {
    if(!m_startedInitialization) {
      GameEngine::getInstance().lockStates();
      m_loadThread = std::thread(&LoadableGameState::startInit, m_gameStateToLoad.get());
      m_startedInitialization = true;
    }

    if(m_gameStateToLoad.get() && m_gameStateToLoad.get()->m_done.load()) {
      GameEngine::getInstance().unlockStates();
      GameEngine::getInstance().popState();

      if(m_gameStateToLoad.get()->m_result.load()) {
        GameEngine::getInstance().addStateInstanceInitialzied(std::move(m_gameStateToLoad));
      }
    }
  }
}
