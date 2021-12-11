#ifndef TILESETTOOLBOX_H
#define TILESETTOOLBOX_H

#include <memory>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <SFML/Graphics/Image.hpp>
#include "../Game/src/core/type.h"

namespace strat_me
{
  class MapEditorCanvas;
  class TilesetToolbox : public QWidget
  {
  public:
    TilesetToolbox(QWidget* parent, MapEditorCanvas* mapEditorCanvas);

  private:
    void openTileFileBrowser();

    void setTileSelection();
    void resetMapTiles();

    void addImageToTileSelection(const sf::Image& image, unsigned int tileCount, std::size_t index);
    void addImageToTileSelection(const sf::Image& image, const gengine2d::string_t& tileName, std::size_t index);

    QGridLayout*                        m_layout;
    QGridLayout*                        m_tileSizeLayout;

    QListWidget*                        m_tileList;
    gengine2d::vector_t<sf::FloatRect>  m_tileRectList;
    QLabel*                             m_tileSizeLabel;
    QTextEdit*                          m_tileSizeTextEdit;
    QPushButton*                        m_tileSizeUpdateButton;

    QPushButton*                        m_loadTilesetButton;
    QPushButton*                        m_importTilesetButton;

    MapEditorCanvas*                    m_mapEditorCanvas;

    unsigned int                        m_tileSize;
  };
}

#endif // TILESETTOOLBOX_H
