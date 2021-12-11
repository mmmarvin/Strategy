#ifndef INPUTEVENT_H
#define INPUTEVENT_H

#include "vector2.h"

namespace gengine2d
{
  struct InputEvent
  {
    enum class EEventType : unsigned char
    {
      ET_KEYBOARD_KEY_PRESSED,
      ET_KEYBOARD_KEY_RELEASED,
      ET_MOUSE_MOVED,
      ET_MOUSE_BUTTON_PRESSED,
      ET_MOUSE_BUTTON_RELEASED,
      ET_MOUSE_WHEEL_SCROLLED
    };

    enum EKeyboardKey : unsigned int
    {
      KK_KEY_A = 0,
      KK_KEY_B,
      KK_KEY_C,
      KK_KEY_D,
      KK_KEY_E,
      KK_KEY_F,
      KK_KEY_G,
      KK_KEY_H,
      KK_KEY_I,
      KK_KEY_J,
      KK_KEY_K,
      KK_KEY_L,
      KK_KEY_M,
      KK_KEY_N,
      KK_KEY_O,
      KK_KEY_P,
      KK_KEY_Q,
      KK_KEY_R,
      KK_KEY_S,
      KK_KEY_T,
      KK_KEY_U,
      KK_KEY_V,
      KK_KEY_W,
      KK_KEY_X,
      KK_KEY_Y,
      KK_KEY_Z,
      KK_KEY_ENTER,
      KK_KEY_LEFT,
      KK_KEY_RIGHT,
      KK_KEY_UP,
      KK_KEY_DOWN,
      KK_KEY_NUM1,
      KK_KEY_NUM2,
      KK_KEY_NUM3,
      KK_KEY_NUM4,
      KK_KEY_NUM5,
      KK_KEY_NUM6,
      KK_KEY_NUM7,
      KK_KEY_NUM8,
      KK_KEY_NUM9,
      KK_KEY_NUM0,
      KK_KEY_NUMPAD1,
      KK_KEY_NUMPAD2,
      KK_KEY_NUMPAD3,
      KK_KEY_NUMPAD4,
      KK_KEY_NUMPAD5,
      KK_KEY_NUMPAD6,
      KK_KEY_NUMPAD7,
      KK_KEY_NUMPAD8,
      KK_KEY_NUMPAD9,
      KK_KEY_NUMPAD0,
      KK_KEY_LCTRL,
      KK_KEY_RCTRL,
      KK_KEY_LSHIFT,
      KK_KEY_RSHIFT,
      KK_KEY_LALT,
      KK_KEY_RALT,
      KK_KEY_TAB,
      KK_KEY_CAPSLOCK,
      KK_KEY_BACKSPACE,
      KK_KEY_SPACE,
      KK_KEY_F1,
      KK_KEY_F2,
      KK_KEY_F3,
      KK_KEY_F4,
      KK_KEY_F5,
      KK_KEY_F6,
      KK_KEY_F7,
      KK_KEY_F8,
      KK_KEY_F9,
      KK_KEY_F10,
      KK_KEY_F11,
      KK_KEY_F12,
      KK_KEY_HOME,
      KK_KEY_END,
      KK_KEY_INSERT,
      KK_KEY_DELETE,
      KK_KEY_PGUP,
      KK_KEY_PGDOWN,
      KK_KEY_ESC,
      KK_KEY_TILDA,
      KK_KEY_COUNT
    };

    enum EMouseButton : unsigned int
    {
      MB_LEFT = 0,
      MB_RIGHT,
      MB_MIDDLE,
      MB_EXTRA_BUTTON1,
      MB_EXTRA_BUTTON2,
      MB_EXTRA_BUTTON3,
      MB_EXTRA_BUTTON4,
      MB_EXTRA_BUTTON5,
      MB_EXTRA_BUTTON6,
      MB_EXTRA_BUTTON7,
      MB_EXTRA_BUTTON8,
      MB_EXTRA_BUTTON9,
      MB_BUTTON_COUNT
    };

    enum EWheelDirection : unsigned char
    {
      WD_UP = 0,
      WD_DOWN
    };

    struct MouseEvent
    {
      gengine2d::Vector2i position;
      EMouseButton        button;
      EWheelDirection     wheelDirection;
    };

    struct KeyboardEvent
    {
      EKeyboardKey        key;
    };

    InputEvent() {}

    EEventType      type;
    union {
      MouseEvent    mouseEvent;
      KeyboardEvent keyboardEvent;
    };
  };
}

#endif // INPUTEVENT_H
