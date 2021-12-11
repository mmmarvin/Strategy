#ifndef MAPEDITORCANVAS_H
#define MAPEDITORCANVAS_H

#include "../Game/src/core/type.h"
#include "../Game/src/game/map/map.h"
#include "../Game/src/game/map/mapproperty.h"
#include "qsfmlcanvas.h"

namespace strat_me
{
  class MapEditorCanvas : public QSFMLCanvas
  {
  public:
    enum class EMode : unsigned char
    {
      MODE_SELECT = 0,
      MODE_MOVE,
      MODE_TILE,
      MODE_OBJECT
    };

    MapEditorCanvas(QWidget* parent, const QSize& size);

    void createNew(unsigned int width, unsigned int height);
    bool loadFromFile(const gengine2d::string_t& filename);
    bool saveToFile(const gengine2d::string_t& filename);
    void resetTileTextureSizes(float width, float height);

    void setMode(EMode mode);

    bool setTileTexture(const gengine2d::string_t& textureFilename);
    void setTileSelectionRect(const sf::FloatRect& selectionRect);

    void showGridLines(bool show);
    void toggleGridLines();
    void setIsometric(bool isometric);

    EMode getMode() const { return m_mode; }
    const sf::Texture& getTileTexture() const { return m_tileTexture; }

    bool gridLines() const { return m_showGridLine; }
    bool isometric() const { return m_isIsometric; }

  private:
    using map_tile_info_type = std::pair<std::array<sf::Vertex, 4>,
                                         std::array<sf::Vertex, 8>>;

    void init() override;
    void draw(sf::RenderWindow& window) override;

    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    void startMove(Qt::MouseButton button, const QPoint& mousePosition);
    void startTile();

    sf::View                                      m_view;

    strat::MapProperty                            m_mapProperty;
    gengine2d::vector_t<map_tile_info_type>       m_tiles;
    sf::Texture                                   m_tileTexture;
    sf::FloatRect                                 m_currentTileSelected;

    gengine2d::Vector2f                           m_dragStartPosition;
    gengine2d::Vector2f                           m_dragDirection;
    Qt::MouseButton                               m_dragButton;

    EMode                                         m_mode;

    bool                                          m_dragStarted;
    bool                                          m_showGridLine;
    bool                                          m_isIsometric;
  };
}

#endif // MAPEDITORCANVAS_H
