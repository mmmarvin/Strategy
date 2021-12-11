#include "sfmlinputhandler.h"

#include <cassert>
#include <imgui.h>
#include <imgui-SFML.h>
#include "../../utility/general_utility.h"
#include "../../type.h"

namespace gengine2d
{
namespace input
{
namespace
{
  InputEvent::EKeyboardKey sfKeyToInputEventKey(sf::Keyboard::Key key)
  {
    static const fast_unordered_map_t<sf::Keyboard::Key, InputEvent::EKeyboardKey> SF_KEY_TO_GENGINE_KEY =
    {
      // TODO: Add more keys
      { sf::Keyboard::A, InputEvent::EKeyboardKey::KK_KEY_A },
      { sf::Keyboard::B, InputEvent::EKeyboardKey::KK_KEY_B },
      { sf::Keyboard::C, InputEvent::EKeyboardKey::KK_KEY_C },
      { sf::Keyboard::D, InputEvent::EKeyboardKey::KK_KEY_D },
      { sf::Keyboard::E, InputEvent::EKeyboardKey::KK_KEY_E },
      { sf::Keyboard::F, InputEvent::EKeyboardKey::KK_KEY_F },
      { sf::Keyboard::G, InputEvent::EKeyboardKey::KK_KEY_G },
      { sf::Keyboard::H, InputEvent::EKeyboardKey::KK_KEY_H },
      { sf::Keyboard::I, InputEvent::EKeyboardKey::KK_KEY_I },
      { sf::Keyboard::J, InputEvent::EKeyboardKey::KK_KEY_J },
      { sf::Keyboard::K, InputEvent::EKeyboardKey::KK_KEY_K },
      { sf::Keyboard::L, InputEvent::EKeyboardKey::KK_KEY_L },
      { sf::Keyboard::M, InputEvent::EKeyboardKey::KK_KEY_M },
      { sf::Keyboard::N, InputEvent::EKeyboardKey::KK_KEY_N },
      { sf::Keyboard::O, InputEvent::EKeyboardKey::KK_KEY_O },
      { sf::Keyboard::P, InputEvent::EKeyboardKey::KK_KEY_P },
      { sf::Keyboard::Q, InputEvent::EKeyboardKey::KK_KEY_Q },
      { sf::Keyboard::R, InputEvent::EKeyboardKey::KK_KEY_R },
      { sf::Keyboard::S, InputEvent::EKeyboardKey::KK_KEY_S },
      { sf::Keyboard::T, InputEvent::EKeyboardKey::KK_KEY_T },
      { sf::Keyboard::U, InputEvent::EKeyboardKey::KK_KEY_U },
      { sf::Keyboard::V, InputEvent::EKeyboardKey::KK_KEY_V },
      { sf::Keyboard::W, InputEvent::EKeyboardKey::KK_KEY_W },
      { sf::Keyboard::X, InputEvent::EKeyboardKey::KK_KEY_X },
      { sf::Keyboard::Y, InputEvent::EKeyboardKey::KK_KEY_Y },
      { sf::Keyboard::Z, InputEvent::EKeyboardKey::KK_KEY_Z },

      { sf::Keyboard::Enter, InputEvent::EKeyboardKey::KK_KEY_ENTER },

      { sf::Keyboard::Num1, InputEvent::EKeyboardKey::KK_KEY_NUM1 },
      { sf::Keyboard::Num2, InputEvent::EKeyboardKey::KK_KEY_NUM2 },
      { sf::Keyboard::Num3, InputEvent::EKeyboardKey::KK_KEY_NUM3 },
      { sf::Keyboard::Num4, InputEvent::EKeyboardKey::KK_KEY_NUM4 },
      { sf::Keyboard::Num5, InputEvent::EKeyboardKey::KK_KEY_NUM5 },
      { sf::Keyboard::Num6, InputEvent::EKeyboardKey::KK_KEY_NUM6 },
      { sf::Keyboard::Num7, InputEvent::EKeyboardKey::KK_KEY_NUM7 },
      { sf::Keyboard::Num8, InputEvent::EKeyboardKey::KK_KEY_NUM8 },
      { sf::Keyboard::Num9, InputEvent::EKeyboardKey::KK_KEY_NUM9 },
      { sf::Keyboard::Num0, InputEvent::EKeyboardKey::KK_KEY_NUM0 },

      { sf::Keyboard::Numpad1, InputEvent::EKeyboardKey::KK_KEY_NUMPAD1 },
      { sf::Keyboard::Numpad2, InputEvent::EKeyboardKey::KK_KEY_NUMPAD2 },
      { sf::Keyboard::Numpad3, InputEvent::EKeyboardKey::KK_KEY_NUMPAD3 },
      { sf::Keyboard::Numpad4, InputEvent::EKeyboardKey::KK_KEY_NUMPAD4 },
      { sf::Keyboard::Numpad5, InputEvent::EKeyboardKey::KK_KEY_NUMPAD5 },
      { sf::Keyboard::Numpad6, InputEvent::EKeyboardKey::KK_KEY_NUMPAD6 },
      { sf::Keyboard::Numpad7, InputEvent::EKeyboardKey::KK_KEY_NUMPAD7 },
      { sf::Keyboard::Numpad8, InputEvent::EKeyboardKey::KK_KEY_NUMPAD8 },
      { sf::Keyboard::Numpad9, InputEvent::EKeyboardKey::KK_KEY_NUMPAD9 },
      { sf::Keyboard::Numpad0, InputEvent::EKeyboardKey::KK_KEY_NUMPAD0 },

      { sf::Keyboard::LControl, InputEvent::EKeyboardKey::KK_KEY_LCTRL },
      { sf::Keyboard::RControl, InputEvent::EKeyboardKey::KK_KEY_RCTRL },
      { sf::Keyboard::LShift, InputEvent::EKeyboardKey::KK_KEY_LSHIFT },
      { sf::Keyboard::RShift, InputEvent::EKeyboardKey::KK_KEY_RSHIFT },
      { sf::Keyboard::LAlt, InputEvent::EKeyboardKey::KK_KEY_LALT },
      { sf::Keyboard::RAlt, InputEvent::EKeyboardKey::KK_KEY_RALT },
      { sf::Keyboard::Tab, InputEvent::EKeyboardKey::KK_KEY_TAB },

      { sf::Keyboard::Backspace, InputEvent::EKeyboardKey::KK_KEY_BACKSPACE },
      { sf::Keyboard::Space, InputEvent::EKeyboardKey::KK_KEY_SPACE },

      { sf::Keyboard::Left, InputEvent::EKeyboardKey::KK_KEY_LEFT },
      { sf::Keyboard::Right, InputEvent::EKeyboardKey::KK_KEY_RIGHT },
      { sf::Keyboard::Up, InputEvent::EKeyboardKey::KK_KEY_UP },
      { sf::Keyboard::Down, InputEvent::EKeyboardKey::KK_KEY_DOWN },

      { sf::Keyboard::F1, InputEvent::EKeyboardKey::KK_KEY_F1 },
      { sf::Keyboard::F2, InputEvent::EKeyboardKey::KK_KEY_F2 },
      { sf::Keyboard::F3, InputEvent::EKeyboardKey::KK_KEY_F3 },
      { sf::Keyboard::F4, InputEvent::EKeyboardKey::KK_KEY_F4 },
      { sf::Keyboard::F5, InputEvent::EKeyboardKey::KK_KEY_F5 },
      { sf::Keyboard::F6, InputEvent::EKeyboardKey::KK_KEY_F6 },
      { sf::Keyboard::F7, InputEvent::EKeyboardKey::KK_KEY_F7 },
      { sf::Keyboard::F8, InputEvent::EKeyboardKey::KK_KEY_F8 },
      { sf::Keyboard::F9, InputEvent::EKeyboardKey::KK_KEY_F9 },
      { sf::Keyboard::F10, InputEvent::EKeyboardKey::KK_KEY_F10 },
      { sf::Keyboard::F11, InputEvent::EKeyboardKey::KK_KEY_F11 },
      { sf::Keyboard::F12, InputEvent::EKeyboardKey::KK_KEY_F12 },

      { sf::Keyboard::Home, InputEvent::EKeyboardKey::KK_KEY_HOME },
      { sf::Keyboard::End, InputEvent::EKeyboardKey::KK_KEY_END },
      { sf::Keyboard::Insert, InputEvent::EKeyboardKey::KK_KEY_INSERT },
      { sf::Keyboard::Delete, InputEvent::EKeyboardKey::KK_KEY_DELETE },
      { sf::Keyboard::PageUp, InputEvent::EKeyboardKey::KK_KEY_PGUP },
      { sf::Keyboard::PageDown, InputEvent::EKeyboardKey::KK_KEY_PGDOWN },
      { sf::Keyboard::Escape, InputEvent::EKeyboardKey::KK_KEY_ESC },
      { sf::Keyboard::Tilde, InputEvent::EKeyboardKey::KK_KEY_TILDA }
    };

    return SF_KEY_TO_GENGINE_KEY.at(key);
  }

  InputEvent::EMouseButton sfButtonToInputEventButton(sf::Mouse::Button button)
  {
    static const fast_unordered_map_t<sf::Mouse::Button, InputEvent::EMouseButton> SF_BUTTON_TO_GENGINE_BUTTON =
    {
      { sf::Mouse::Button::Left, InputEvent::EMouseButton::MB_LEFT },
      { sf::Mouse::Button::Right, InputEvent::EMouseButton::MB_RIGHT },
      { sf::Mouse::Button::Middle, InputEvent::EMouseButton::MB_MIDDLE },
      { sf::Mouse::Button::XButton1, InputEvent::EMouseButton::MB_EXTRA_BUTTON1 },
      { sf::Mouse::Button::XButton2, InputEvent::EMouseButton::MB_EXTRA_BUTTON2 }
    };

    return SF_BUTTON_TO_GENGINE_BUTTON.at(button);
  }
}
  SFMLInputHandler::SFMLInputHandler() :
    m_renderWindow(nullptr)
  {
#ifdef NDEBUG
    // disable logging of sfml
    sf::err().rdbuf(nullptr);
#endif // NDEBUG
  }

  bool SFMLInputHandler::handleInput(InputEvent& event)
  {
    assert(m_renderWindow);
    sf::Event sfEvent;

    while(m_renderWindow->pollEvent(sfEvent)) {
      switch(sfEvent.type) {
      case sf::Event::Closed:
        m_renderWindow->close();
        return false;
      case sf::Event::KeyPressed:
        event.type = InputEvent::EEventType::ET_KEYBOARD_KEY_PRESSED;
        event.keyboardEvent.key = sfKeyToInputEventKey(sfEvent.key.code);
        break;
      case sf::Event::KeyReleased:
        event.type = InputEvent::EEventType::ET_KEYBOARD_KEY_RELEASED;
        event.keyboardEvent.key = sfKeyToInputEventKey(sfEvent.key.code);
        break;
      case sf::Event::MouseButtonPressed:
        event.type = InputEvent::EEventType::ET_MOUSE_BUTTON_PRESSED;
        event.mouseEvent.button = sfButtonToInputEventButton(sfEvent.mouseButton.button);
        break;
      case sf::Event::MouseButtonReleased:
        event.type = InputEvent::EEventType::ET_MOUSE_BUTTON_RELEASED;
        event.mouseEvent.button = sfButtonToInputEventButton(sfEvent.mouseButton.button);
        break;
      case sf::Event::MouseMoved:
        event.type = InputEvent::EEventType::ET_MOUSE_MOVED;
        event.mouseEvent.position = sf::Mouse::getPosition(*m_renderWindow);
        break;
      case sf::Event::MouseWheelScrolled:
        event.type = InputEvent::EEventType::ET_MOUSE_WHEEL_SCROLLED;
        event.mouseEvent.wheelDirection = sfEvent.mouseWheelScroll.delta > 0.f ?
                                          InputEvent::EWheelDirection::WD_UP :
                                          InputEvent::EWheelDirection::WD_DOWN;
        break;
      default:
        // Let ImGui-SFML Process the event(s) that gengine ignores
        ImGui::SFML::ProcessEvent(sfEvent);
        continue;
      }

      ImGui::SFML::ProcessEvent(sfEvent);
      return true;
    }

    return false;
  }

  gengine2d::Vector2f SFMLInputHandler::getMousePosition() const
  {
    auto mousePosition = sf::Mouse::getPosition(*m_renderWindow);
    return gengine2d::Vector2f(mousePosition.x, mousePosition.y);
  }
}
}
