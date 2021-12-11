#include "unitselectioncreator.h"

#include "../../core/debug/debug_log.h"
#include "../../core/utility/stl_utility.h"
#include "../../core/utility/vector_utility.h"
#include "../../core/game.h"
#include "../common/position_common.h"
#include "../game_def.h"

namespace strat
{
  UnitSelectionCreator::UnitSelectionCreator(const gengine2d::InputState& inputState) :
    m_inputState(&inputState),
    m_selectionStarted(false)
  {
    m_selectionStyle.fillColor = gengine2d::Color(0, 255, 0, 32);
    m_selectionStyle.outlineColor = gengine2d::Color(0, 255, 0);
    m_selectionStyle.outlineThickness = 1;

    m_selectionPositionStyle.font = &gengine2d::GameEngine::getInstance().getResourceManager().getFontLookup().getResource("FONT_DEFAULT");
    m_selectionPositionStyle.characterSize = 12;
    m_selectionPositionStyle.fillColor = gengine2d::Color::White;
    m_selectionPositionStyle.outlineColor = gengine2d::Color::Black;
    m_selectionPositionStyle.outlineThickness = 1.f;
//    m_selectionPositionStyle.style = gengine2d::Text2DStyle::TS_BOLD;
  }

  void UnitSelectionCreator::draw(bool drawDebugPosition)
  {
    if(m_selectionStarted) {
      auto selectionDimension = m_selectionEnd - m_selectionStart;
      gengine2d::GameEngine::getInstance().getRenderer().draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(m_selectionStart.x, m_selectionStart.y, selectionDimension.x, selectionDimension.y),
                                                                    m_selectionStyle);

      if(drawDebugPosition) {
        auto dim = m_selectionEnd - m_selectionStart;
        drawPositionText(m_selectionStart, gengine2d::Vector2f(0.f, -10.f));
        drawPositionText(m_selectionStart + gengine2d::Vector2f(dim.x, 0.f),
                         gengine2d::Vector2f(0.f, -10.f));
        drawPositionText(m_selectionEnd, gengine2d::Vector2f(0.f, 10.f));
        drawPositionText(m_selectionStart + gengine2d::Vector2f(0.f, dim.y),
                         gengine2d::Vector2f(0.f, 10.f));
      }
    }
  }

  bool UnitSelectionCreator::handleInput(const gengine2d::InputEvent& event)
  {
    const auto& mousePosition = m_inputState->getMousePosition(1);

    if(event.type == gengine2d::InputEvent::EEventType::ET_MOUSE_BUTTON_PRESSED &&
       event.mouseEvent.button == gengine2d::InputEvent::EMouseButton::MB_LEFT &&
       !m_selectionStarted) {
      m_selectionStarted = true;
      m_selectionStart = m_selectionEnd = mousePosition;
    } else if(event.type == gengine2d::InputEvent::EEventType::ET_MOUSE_BUTTON_RELEASED &&
              event.mouseEvent.button == gengine2d::InputEvent::EMouseButton::MB_LEFT &&
              m_selectionStarted) {
      m_selectionStarted = false;
      findSelectionSquarePoints();

      return true;
    } else if(event.type == gengine2d::InputEvent::EEventType::ET_MOUSE_MOVED && m_selectionStarted) {
      m_selectionEnd = mousePosition;
    }

    return false;
  }

  std::pair<gengine2d::Vector2f, gengine2d::Vector2f> UnitSelectionCreator::getSelection()
  {
    auto ret = std::make_pair(m_selectionStart, m_selectionEnd);
    resetSelection();

    return ret;
  }

  void UnitSelectionCreator::resetSelection()
  {
    m_selectionStart = m_selectionEnd = gengine2d::Vector2f(0.f, 0.f);
  }

  void UnitSelectionCreator::drawPositionText(const gengine2d::Vector2f& position,
                                              const gengine2d::Vector2f& textPositionOffset)
  {
    auto selectionPositionString = position_common::getIsoPositionString(position,
                                                                         gengine2d::vector_utils::convertIsoToCartPoint(position) / TILE_SIZE);
    auto selectionPositionBounds = gengine2d::GameEngine::getInstance().getRenderer().calculate2DTextBounds(selectionPositionString,
                                                                                                            m_selectionPositionStyle);

    gengine2d::GameEngine::getInstance().getRenderer().draw2DText(gengine2d::Vector2<gengine2d::render::position_point_2d_t>(position.x + textPositionOffset.x - (selectionPositionBounds.x / 2.f),
                                                                                                                             position.y + textPositionOffset.y - (selectionPositionBounds.y / 2.f)),
                                                                  selectionPositionString,
                                                                  m_selectionPositionStyle);
  }

  void UnitSelectionCreator::findSelectionSquarePoints()
  {
    if(m_selectionStart.x > m_selectionEnd.x &&
       m_selectionStart.y > m_selectionEnd.y) {
      std::swap(m_selectionStart, m_selectionEnd);
    } else if(m_selectionStart.x < m_selectionEnd.x &&
              m_selectionStart.y > m_selectionEnd.y) {
      auto diff = m_selectionEnd - m_selectionStart;
      auto p1 = m_selectionStart + gengine2d::Vector2f(0, diff.y);
      auto p2 = m_selectionEnd - gengine2d::Vector2f(0, diff.y);
      m_selectionStart = p1;
      m_selectionEnd = p2;
    } else if(m_selectionStart.x > m_selectionEnd.x &&
              m_selectionStart.y < m_selectionEnd.y) {
      auto diff = m_selectionEnd - m_selectionStart;
      auto p1 = m_selectionEnd - gengine2d::Vector2f(0, diff.y);
      auto p2 = m_selectionStart + gengine2d::Vector2f(0, diff.y);
      m_selectionStart = p1;
      m_selectionEnd = p2;
    }
  }
}
