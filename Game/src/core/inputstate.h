#ifndef INPUTSTATE_H
#define INPUTSTATE_H

#include <SFML/Window/Keyboard.hpp>
#include "inputevent.h"
#include "vector2.h"
#include "type.h"

namespace gengine2d
{
  class GameState;
  class InputState
  {
  public:
    InputState(std::size_t numberOfViews);

    bool isKeyboardKeyDown(InputEvent::EKeyboardKey key) const;
    bool isKeyboardKeyUp(InputEvent::EKeyboardKey key) const;

    bool isMouseButtonDown(InputEvent::EMouseButton button) const;
    bool isMouseButtonUp(InputEvent::EMouseButton button) const;

    const gengine2d::Vector2f& getMousePosition(std::size_t viewId) const;

  private:
    friend class GameState;

    vector_t<gengine2d::Vector2f> m_mousePositions;
    vector_t<bool>                m_mouseButtonState;

    vector_t<bool>                m_keyboardKeyState;
  };
}

#endif // INPUTSTATE_H
