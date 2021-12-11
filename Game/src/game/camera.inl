#include "camera.h"

#include "../../../core/styles/shape2dstyle.h"
#include "../../../core/game.h"

namespace strat
{
  template<class PositionChangerFunc>
  void Camera::drawMiniMap(float xRatio,
                           float yRatio,
                           PositionChangerFunc positionChangerFunc) const
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    auto viewSize = m_view.getSize();
    auto viewPosition = m_view.getCenter();

    viewSize.x *= xRatio;
    viewSize.y *= yRatio;
    viewPosition.x *= xRatio;
    viewPosition.y *= yRatio;

    auto cameraPosition = viewPosition - gengine2d::Vector2f(viewSize.x / 2.f, viewSize.y / 2.f);
    gengine2d::Shape2DStyle cameraStyle;
    cameraStyle.fillColor = gengine2d::Color::Transparent;
    cameraStyle.outlineColor = gengine2d::Color::White;
    cameraStyle.outlineThickness = 2;

    renderer.draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(cameraPosition.x,
                                                                                cameraPosition.y,
                                                                                viewSize.x,
                                                                                viewSize.y),
                        cameraStyle,
                        positionChangerFunc);
  }
}
