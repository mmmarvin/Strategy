#include "inputstate.h"

#include <cassert>
#include "inputevent.h"

namespace gengine2d
{
  InputState::InputState(std::size_t numberOfViews)
  {
    m_mousePositions.resize(numberOfViews + 1);
    m_mouseButtonState.resize(gengine2d::InputEvent::EMouseButton::MB_BUTTON_COUNT, false);

    m_keyboardKeyState.resize(gengine2d::InputEvent::EKeyboardKey::KK_KEY_COUNT, false);

  }

  bool InputState::isKeyboardKeyDown(InputEvent::EKeyboardKey key) const
  {
    assert(key < m_keyboardKeyState.size());
    return m_keyboardKeyState[key];
  }

  bool InputState::isKeyboardKeyUp(InputEvent::EKeyboardKey key) const
  {
    assert(key < m_keyboardKeyState.size());
    return !isKeyboardKeyDown(key);
  }

  bool InputState::isMouseButtonDown(InputEvent::EMouseButton button) const
  {
    assert(button < m_mouseButtonState.size());
    return m_mouseButtonState[button];
  }

  bool InputState::isMouseButtonUp(InputEvent::EMouseButton button) const
  {
    assert(button < m_mouseButtonState.size());
    return !isMouseButtonDown(button);
  }

  const gengine2d::Vector2f& InputState::getMousePosition(std::size_t viewId) const
  {
    assert(viewId < m_mousePositions.size());
    return m_mousePositions[viewId];
  }
}
