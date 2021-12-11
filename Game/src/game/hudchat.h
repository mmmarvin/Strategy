#ifndef HUDCHAT_H
#define HUDCHAT_H

#include "../core/3rdparty/high_resolution_clock.h"
#include "../core/type.h"

namespace strat
{
  class HUDChat
  {
  public:
    HUDChat();

    void draw();

    void pushMessage(gengine2d::string_t to, gengine2d::string_t from, gengine2d::string_t message);

    void setMessageMode(bool mode);
    bool isMessageMode() const { return m_messageMode; }

  private:
    gengine2d::vector_t<std::tuple<gengine2d::string_t,
                                   gengine2d::string_t,
                                   gengine2d::string_t>>                      m_messages;
    std::size_t                                                               m_maxMessages;

    mar::high_resolution_stopwatch                                            m_durationClock;
    bool                                                                      m_messageMode;
  };
}

#endif // HUDCHAT_H
