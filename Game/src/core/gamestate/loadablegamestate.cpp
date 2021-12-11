#include "loadablegamestate.h"

namespace gengine2d
{
  LoadableGameState::LoadableGameState(std::initializer_list<gengine2d::game_internal::Renderer::Camera2D*> cameras,
                                       std::initializer_list<std::tuple<gengine2d::string_t, bool, unsigned int, std::function<void(gengine2d::GameState*)>>> initialKeyMapped) :
    gengine2d::GameState(cameras, initialKeyMapped),
    m_percentage(0),
    m_done(false),
    m_result(false)
  {
  }

  void LoadableGameState::setLoadPercentage(int percentage)
  {
    m_percentage.store(percentage);
  }

  int LoadableGameState::getPercentage() const
  {
    return m_percentage.load();
  }

  void LoadableGameState::startInit()
  {
    auto initResult = this->init();

    m_result.store(initResult);
    m_done.store(true);
  }
}
