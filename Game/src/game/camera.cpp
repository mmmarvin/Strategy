#include "camera.h"

#include "../core/game.h"
#include "../core/utility/general_utility.h"
#include "../core/utility/vector_utility.h"
#include "game_def.h"

namespace strat
{
  Camera::Camera() :
    m_cameraMovementMultiplier(0.f, 0.f)
  {
  }

  Camera::Camera(float viewWidth, float viewHeight) :
    m_cameraMovementMultiplier(0.f, 0.f)
  {
    m_view.setSize(gengine2d::Vector2f(viewWidth, viewHeight));
  }

  void Camera::update(float dt, float mapWidth, float mapHeight, const gengine2d::Vector2f& mousePosition)
  {
    const auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    const auto& screenSize = renderer.get2DGUICamera().getSize();

    if(mousePosition.x <= 50) {
      if(mousePosition.x <= 10) {
        moveScreen(gengine2d::Vector2f(-2000, 0) * dt, mapWidth, mapHeight);
      } else {
        moveScreen(gengine2d::Vector2f(-100, 0) * dt, mapWidth, mapHeight);
      }
    } else if(mousePosition.x >= screenSize.x - 50) {
      if(mousePosition.x >= screenSize.x - 10) {
        moveScreen(gengine2d::Vector2f(2000, 0) * dt, mapWidth, mapHeight);
      } else {
        moveScreen(gengine2d::Vector2f(100, 0) * dt, mapWidth, mapHeight);
      }
    }

    if(mousePosition.y <= 50) {
      if(mousePosition.y <= 10) {
        moveScreen(gengine2d::Vector2f(0, -2000) * dt, mapWidth, mapHeight);
      } else {
        moveScreen(gengine2d::Vector2f(0, -100) * dt, mapWidth, mapHeight);
      }
    } else if(mousePosition.y >= screenSize.y - 50) {
      if(mousePosition.y >= screenSize.y - 10) {
        moveScreen(gengine2d::Vector2f(0, 2000) * dt, mapWidth, mapHeight);
      } else {
        moveScreen(gengine2d::Vector2f(0, 100) * dt, mapWidth, mapHeight);
      }
    }
  }

  void Camera::moveLeft(float dt, float mapWidth, float mapHeight)
  {
    if(m_cameraMovementMultiplier.x > 0.f) {
      m_cameraMovementMultiplier.x = -0.1f;
    } else {
      m_cameraMovementMultiplier.x = gengine2d::general_utils::max(-1.f, m_cameraMovementMultiplier.x - 0.1f);
    }

    moveScreenX(-1000 * -m_cameraMovementMultiplier.x * dt, mapWidth, mapHeight);
  }

  void Camera::moveRight(float dt, float mapWidth, float mapHeight)
  {
    if(m_cameraMovementMultiplier.x < 0.f) {
      m_cameraMovementMultiplier.x = 0.1f;
    } else {
      m_cameraMovementMultiplier.x = gengine2d::general_utils::min(1.f, m_cameraMovementMultiplier.x + 0.1f);
    }

    moveScreenX(1000 * m_cameraMovementMultiplier.x * dt, mapWidth, mapHeight);
  }

  void Camera::moveUp(float dt, float mapWidth, float mapHeight)
  {
    if(m_cameraMovementMultiplier.y > 0.f) {
      m_cameraMovementMultiplier.y = -0.1f;
    } else {
      m_cameraMovementMultiplier.y = gengine2d::general_utils::max(-1.f, m_cameraMovementMultiplier.y - 0.1f);
    }

    moveScreenY(-1000 * -m_cameraMovementMultiplier.y * dt, mapWidth, mapHeight);
  }

  void Camera::moveDown(float dt, float mapWidth, float mapHeight)
  {
    if(m_cameraMovementMultiplier.y < 0.f) {
      m_cameraMovementMultiplier.y = 0.1f;
    } else {
      m_cameraMovementMultiplier.y = gengine2d::general_utils::min(1.f, m_cameraMovementMultiplier.y + 0.1f);
    }

    moveScreenY(1000 * m_cameraMovementMultiplier.y * dt, mapWidth, mapHeight);
  }

  void Camera::zoomIn()
  {
    if(getSize().x > CAMERA_SIZE_X_MIN) {
      m_view.zoom(0.95f);
    }
  }

  void Camera::zoomOut()
  {
    if(getSize().x < CAMERA_SIZE_X_MAX) {
      m_view.zoom(1.05f);
    }
  }

  void Camera::zoomNormal()
  {
    setSize(gengine2d::Vector2f(CAMERA_SIZE_X, CAMERA_SIZE_Y)) ;
  }

  void Camera::setSize(const gengine2d::Vector2f& size)
  {
    m_view.setSize(size);
  }

  void Camera::setCenter(const gengine2d::Vector2f& center)
  {
    m_view.setCenter(center);
  }

  void Camera::moveScreen(const gengine2d::Vector2f& direction, float mapWidth, float mapHeight)
  {
    const auto& viewSize = m_view.getSize();

    // TODO: Change to map size
    auto newCenter = m_view.getCenter() + direction;

    newCenter = gengine2d::vector_utils::convertIsoToCartPoint(newCenter);
    newCenter.x = gengine2d::general_utils::clamp(newCenter.x,
                                                  (viewSize.x / 4.f),
                                                  (mapWidth * TILE_SIZE) - (viewSize.x / 4.f));
    newCenter.y = gengine2d::general_utils::clamp(newCenter.y,
                                                  (viewSize.y / 4.f),
                                                  (mapHeight * TILE_SIZE) - (viewSize.y / 4.f));
    m_view.setCenter(gengine2d::vector_utils::convertCartToIsoPoint(newCenter));
  }

  void Camera::moveScreenX(float value, float mapWidth, float mapHeight)
  {
    const auto& viewSize = m_view.getSize();

    auto newCenter = m_view.getCenter();
    newCenter.x += value;

    newCenter = gengine2d::vector_utils::convertIsoToCartPoint(newCenter);
    newCenter.x = gengine2d::general_utils::clamp(newCenter.x,
                                                  (viewSize.x / 4.f),
                                                  (mapWidth * TILE_SIZE) - (viewSize.x / 4.f));
    newCenter.y = gengine2d::general_utils::clamp(newCenter.y,
                                                  (viewSize.y / 4.f),
                                                  (mapHeight * TILE_SIZE) - (viewSize.y / 4.f));
    m_view.setCenter(gengine2d::vector_utils::convertCartToIsoPoint(newCenter));
  }

  void Camera::moveScreenY(float value, float mapWidth, float mapHeight)
  {
    const auto& viewSize = m_view.getSize();

    auto newCenter = m_view.getCenter();
    newCenter.y += value;

    newCenter = gengine2d::vector_utils::convertIsoToCartPoint(newCenter);
    newCenter.x = gengine2d::general_utils::clamp(newCenter.x,
                                                  (viewSize.x / 4.f),
                                                  (mapWidth * TILE_SIZE) - (viewSize.x / 4.f));
    newCenter.y = gengine2d::general_utils::clamp(newCenter.y,
                                                  (viewSize.y / 4.f),
                                                  (mapHeight * TILE_SIZE) - (viewSize.x / 4.f));
    m_view.setCenter(gengine2d::vector_utils::convertCartToIsoPoint(newCenter));
  }
}
