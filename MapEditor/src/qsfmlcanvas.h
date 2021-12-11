#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <QMainWindow>
#include <QTimer>
#include <SFML/Graphics.hpp>
#include "../Game/src/core/vector2.h"

namespace strat_me
{
  class QSFMLCanvas : public QWidget
  {
  public:
    QSFMLCanvas(QWidget* parent,
                const QSize& size,
                unsigned int drawRate = 64);
    ~QSFMLCanvas();

  protected:
    virtual void init() = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    sf::RenderWindow& getRenderWindow() { return m_renderWindow; }

  protected:
    QPaintEngine* paintEngine() const override;

    void showEvent(QShowEvent*) override;
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent* event) override;

    sf::RenderWindow  m_renderWindow;
    QTimer            m_timer;
    bool              m_initialized;
  };
}

#endif // QSFMLCANVAS_H
