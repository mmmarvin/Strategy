#ifndef LOADINGGAMESTATE_H
#define LOADINGGAMESTATE_H

#include <thread>
#include "../gamestate.h"
#include "loadablegamestate.h"

namespace gengine2d
{
  class LoadingGameState : public GameState
  {
  public:
    explicit LoadingGameState(std::unique_ptr<LoadableGameState> gameStateToLoad);
    ~LoadingGameState();

    gengine2d::string_t getDebugName() const override final { return "LOADING_STATE"; }

    void update(float) override;
    void interpolate(float, float) override final {}
    void handleInput(const InputEvent&) override final {}

  protected:
    bool isLoading() const { return m_gameStateToLoad.get() != nullptr; }
    const std::unique_ptr<LoadableGameState>& getGameStateToLoad() const { return m_gameStateToLoad; }

  private:
    std::thread                         m_loadThread;
    std::unique_ptr<LoadableGameState>  m_gameStateToLoad;
    bool                                m_startedInitialization;
  };
}

#endif // LOADINGGAMESTATE_H
