#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>
#include "../core/game_internal_types.h"
#include "../core/vector2.h"

namespace strat
{
  class Camera
  {
  public:
    Camera();
    Camera(float viewWidth, float viewHeight);

    template<class PositionChangerFunc>
    void drawMiniMap(float xRatio,
                     float yRatio,
                     PositionChangerFunc positionChangerFunc) const;
    void update(float dt, float mapWidth, float mapHeight, const gengine2d::Vector2f& mousePosition);

    void moveLeft(float dt, float mapWidth, float mapHeight);
    void moveRight(float dt, float mapWidth, float mapHeight);
    void moveUp(float dt, float mapWidth, float mapHeight);
    void moveDown(float dt, float mapWidth, float mapHeight);

    void zoomIn();
    void zoomOut();
    void zoomNormal();

    void setSize(const gengine2d::Vector2f& size);
    gengine2d::Vector2f getSize() const { return m_view.getSize(); }
    void setCenter(const gengine2d::Vector2f& center);
    gengine2d::Vector2f getCenter() const { return m_view.getCenter(); }

    gengine2d::game_internal::Renderer::Camera2D& getView() { return m_view; }
    const gengine2d::game_internal::Renderer::Camera2D& getView() const { return m_view; }

  private:
    void moveScreen(const gengine2d::Vector2f& direction, float mapWidth, float mapHeight);
    void moveScreenX(float value, float mapWidth, float mapHeight);
    void moveScreenY(float value, float mapWidth, float mapHeight);

    gengine2d::Vector2f                           m_cameraMovementMultiplier;
    gengine2d::game_internal::Renderer::Camera2D  m_view;
  };
}

#include "camera.inl"

#endif // CAMERA_H
