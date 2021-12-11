#include <QResizeEvent>
#include "qsfmlcanvas.h"

namespace strat_me
{
  QSFMLCanvas::QSFMLCanvas(QWidget* parent,
                           const QSize& size,
                           unsigned int drawRate) :
    QWidget(parent),
    m_initialized(false)
  {
    this->setMouseTracking(true);

    this->setAttribute(Qt::WA_PaintOnScreen);
    this->setAttribute(Qt::WA_OpaquePaintEvent);
    this->setAttribute(Qt::WA_NoSystemBackground);
    this->setFocusPolicy(Qt::StrongFocus);

    this->resize(size);

    m_timer.setInterval(drawRate);
  }

  QSFMLCanvas::~QSFMLCanvas()
  {
    m_renderWindow.close();
  }

  QPaintEngine* QSFMLCanvas::paintEngine() const
  {
    return nullptr;
  }

  void QSFMLCanvas::showEvent(QShowEvent*)
  {
    if(!m_initialized) {
      m_renderWindow.create(winId());
      init();

      connect(&m_timer, SIGNAL(timeout()), this, SLOT(repaint()));
      m_timer.start();

      m_initialized = true;
    }
  }

  void QSFMLCanvas::paintEvent(QPaintEvent*)
  {
    m_renderWindow.clear();
    draw(m_renderWindow);
    m_renderWindow.display();
  }

  void QSFMLCanvas::resizeEvent(QResizeEvent* event)
  {
    m_renderWindow.setSize(sf::Vector2u(event->size().width(), event->size().height()));
  }
}
