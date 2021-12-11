#ifndef LOADABLEGAMESTATE_H
#define LOADABLEGAMESTATE_H

#include <atomic>
#include "../gamestate.h"

namespace gengine2d
{
  class LoadingGameState;
  class LoadableGameState : public gengine2d::GameState
  {
  protected:
    LoadableGameState(std::initializer_list<gengine2d::game_internal::Renderer::Camera2D*> cameras,
                      std::initializer_list<std::tuple<gengine2d::string_t, bool, unsigned int, std::function<void(gengine2d::GameState*)>>> initialKeyMapped);

    void setLoadPercentage(int percentage);

  public:
    int getPercentage() const;

  private:
    friend class LoadingGameState;

    void startInit();

    std::atomic_int   m_percentage;
    std::atomic_bool  m_done;
    std::atomic_bool  m_result;
  };
}

#endif // LOADABLEGAMESTATE_H
