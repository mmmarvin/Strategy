#include "hudmessage.h"

#include "../core/game.h"
#include "game_def.h"

namespace strat
{
  bool HUDMessage::init()
  {
    const auto& fontLookup = gengine2d::GameEngine::getInstance().getResourceManager().getFontLookup();

    m_messageStyle.characterSize = 24;
    m_messageStyle.font = &fontLookup.getResource("FONT_DEFAULT");

    return true;
  }

  void HUDMessage::draw()
  {
    if(m_messageDurationClock.is_running()) {
      if(m_messageDurationClock.get_elapsed().as_seconds() <= 5.f) {
        gengine2d::GameEngine::getInstance().getRenderer().draw2DText(m_messagePosition,
                                                                      m_message,
                                                                      m_messageStyle);
      } else {
        m_messageDurationClock.stop();
      }
    }
  }

  void HUDMessage::showMessage(const gengine2d::string_t& msg)
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    auto [renderWidth, renderHeight] = renderer.getScreenSize();
    auto messageBounds = renderer.calculate2DTextBounds(msg, m_messageStyle);

    m_message = msg;
    m_messagePosition = gengine2d::Vector2<gengine2d::render::position_point_2d_t>((renderWidth / 2.f) -
                                                                                   (messageBounds.x / 2.f),
                                                                                   MINIMAP_SIZE + 15.f);
    m_messageDurationClock.restart();
  }
}
