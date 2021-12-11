#ifndef HUDMESSAGE_H
#define HUDMESSAGE_H

#include <SFML/Graphics/Text.hpp>
#include "../core/3rdparty/high_resolution_clock.h"
#include "../core/styles/text2dstyle.h"
#include "../core/type.h"

namespace strat
{
  class HUDMessage
  {
  public:
    HUDMessage() {}
    bool init();

    void draw();

    void showMessage(const gengine2d::string_t& msg);

  private:
    gengine2d::string_t                                         m_message;
    gengine2d::Text2DStyle                                      m_messageStyle;
    gengine2d::Vector2<gengine2d::render::position_point_2d_t>  m_messagePosition;
    mar::high_resolution_stopwatch                              m_messageDurationClock;
  };
}

#endif // HUDMESSAGE_H
