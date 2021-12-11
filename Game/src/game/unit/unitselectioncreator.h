#ifndef UNITSELECTORCREATOR_H
#define UNITSELECTORCREATOR_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "../../core/styles/shape2dstyle.h"
#include "../../core/styles/text2dstyle.h"
#include "../../core/inputstate.h"
#include "../../core/vector2.h"

namespace strat
{
  class UnitSelectionCreator
  {
  public:
    UnitSelectionCreator(const gengine2d::InputState& inputState);

    void draw(bool drawDebugPosition);
    bool handleInput(const gengine2d::InputEvent& event);

    std::pair<gengine2d::Vector2f, gengine2d::Vector2f> getSelection();
    void resetSelection();

    bool selectionStarted() const { return m_selectionStarted; }

  private:
    void drawPositionText(const gengine2d::Vector2f& position,
                          const gengine2d::Vector2f& textPositionOffset);
    void findSelectionSquarePoints();

    const gengine2d::InputState*  m_inputState;

    gengine2d::Shape2DStyle       m_selectionStyle;
    gengine2d::Text2DStyle        m_selectionPositionStyle;

    gengine2d::Vector2f           m_selectionStart;
    gengine2d::Vector2f           m_selectionEnd;
    bool                          m_selectionStarted;
  };
}

#endif // UNITSELECTORCREATOR_H
