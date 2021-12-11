#ifndef SFMLINPUTHANDLER_H
#define SFMLINPUTHANDLER_H

#include <SFML/Graphics.hpp>
#include "../../inputevent.h"

namespace gengine2d
{
namespace render
{
  class SFMLRenderer;
}
namespace input
{
  class SFMLInputHandler
  {
  public:
    SFMLInputHandler();

    bool handleInput(InputEvent& event);
    gengine2d::Vector2f getMousePosition() const;

    void ImGuiHandleInput() {}

  private:
    friend class ::gengine2d::render::SFMLRenderer;

    sf::RenderWindow* m_renderWindow;
  };
}
}

#endif // SFMLINPUTHANDLER_H
