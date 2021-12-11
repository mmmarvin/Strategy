#ifndef INGAMELOADINGGAMESTATE_H
#define INGAMELOADINGGAMESTATE_H

#include "../core/gamestate/loadinggamestate.h"

namespace strat
{
  class InGameLoadingGameState : public gengine2d::LoadingGameState
  {
  public:
    explicit InGameLoadingGameState(std::unique_ptr<gengine2d::LoadableGameState> gameStateToLoad);

    bool init() override final;

    void draw(float) override final;
    void update(float) override final;

  private:
    sf::RectangleShape  m_background;
    sf::RectangleShape  m_loadingBar;
    sf::RectangleShape  m_loadingBarBg;

    sf::Texture         m_backgroundTexture;
    sf::Texture         m_loadingBarTexture;
    sf::Texture         m_loadingBarBgTexture;

    float               m_loadingBarWidth;
    float               m_loadingBarHeight;
  };
}

#endif // INGAMELOADINGGAMESTATE_H
