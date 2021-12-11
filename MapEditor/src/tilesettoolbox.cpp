#include <QAction>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include "../Game/src/core/debug/debug_log.h"
#include "../Game/src/core/utility/converter_utility.h"
#include "../Game/src/core/utility/general_utility.h"
#include "../Game/src/core/utility/stl_utility.h"
#include "../Game/src/game/game_def.h"
#include "mapeditorcanvas.h"
#include "tilesettoolbox.h"

namespace strat_me
{
namespace
{
  QImage sfImageToQtImage(const sf::Image& image)
  {
    return QImage(image.getPixelsPtr(),
                  image.getSize().x,
                  image.getSize().y,
                  QImage::Format::Format_RGBA8888);
  }

  QIcon qtImageToQtIcon(const QImage& image)
  {
    QPixmap pixMap;
    pixMap.convertFromImage(image);

    return QIcon(pixMap);
  }

  sf::Image copyImageRectToImage(const sf::Image& image, const sf::FloatRect& rect)
  {
    // brute-force copy every pixel of the image to crop
    sf::Image croppedImage;
    croppedImage.create(rect.width, rect.height, sf::Color(255, 255, 255));

    for(unsigned int j = 0; j < rect.height; ++j) {
      for(unsigned int i = 0; i < rect.width; ++i) {
        auto imagePixelColor = image.getPixel(rect.left + i, rect.top + j);
        croppedImage.setPixel(i, j, imagePixelColor);
      }
    }

    return croppedImage;
  }

  gengine2d::string_t getTileCountString(int count)
  {
    if(count < 10) {
      return "0" + gengine2d::stl_utils::to_string(count);
    }

    return gengine2d::stl_utils::to_string(count);
  }
}
  TilesetToolbox::TilesetToolbox(QWidget* parent, MapEditorCanvas* mapEditorCanvas) :
    QWidget(parent),
    m_mapEditorCanvas(mapEditorCanvas),
    m_tileSize(strat::TILE_SIZE)
  {
    m_layout = new QGridLayout(this);
    m_tileList = new QListWidget(this);
    m_tileList->setIconSize(QSize(64, 64));
    connect(m_tileList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem* item) {
      int index = item->data(Qt::UserRole).toInt();
      assert(index >= 0 && index < m_tileRectList.size());
      m_mapEditorCanvas->setTileSelectionRect(m_tileRectList[index]);
    });
    m_tileSizeLayout = new QGridLayout;

    m_layout->addWidget(m_tileList, 0, 0, 1, 1);
    m_layout->addLayout(m_tileSizeLayout, 1, 0, 1, 1);

    m_tileSizeLabel = new QLabel(this);
    m_tileSizeLabel->setText("Tile Size:");
    m_tileSizeTextEdit = new QTextEdit(this);
    m_tileSizeTextEdit->setText("64");
    // TODO: Change this to get better size for text edit
    m_tileSizeTextEdit->setFixedHeight(gengine2d::general_utils::max(m_tileSizeTextEdit->font().pixelSize(),
                                                                     m_tileSizeTextEdit->font().pointSize()) * 3);
    m_tileSizeUpdateButton = new QPushButton(this);
    m_tileSizeUpdateButton->setText("Update");
    connect(m_tileSizeUpdateButton, &QPushButton::clicked, [this](){
      auto text = m_tileSizeTextEdit->toPlainText();
      if(text.size() > 0) {
        auto tileSize = gengine2d::converter_utils::stringTo<int>(text.toUtf8().data());
        if(tileSize > 0) {
          m_tileSize = tileSize;
          if(m_tileList->count() > 0) {
            setTileSelection();
          }
        } else {
          QMessageBox::critical(this, "Error", "Invalid tile size! Size must be > 0", QMessageBox::Button::Ok);
          m_tileSize = strat::TILE_SIZE;
          m_tileSizeTextEdit->setText(gengine2d::stl_utils::to_string(m_tileSize).c_str());
          if(m_tileList->count() > 0) {
            setTileSelection();
          }
        }
      }
    });

    m_tileSizeLayout->addWidget(m_tileSizeLabel, 0, 0, 1, 1);
    m_tileSizeLayout->addWidget(m_tileSizeTextEdit, 0, 1, 1, 1);
    m_tileSizeLayout->addWidget(m_tileSizeUpdateButton, 0, 2, 1, 1);

    m_loadTilesetButton = new QPushButton(this);
    m_loadTilesetButton->setText("Load Tileset...");
    m_loadTilesetButton->setEnabled(false);
    m_importTilesetButton = new QPushButton(this);
    m_importTilesetButton->setText("Import Tileset...");
    connect(m_importTilesetButton, &QPushButton::clicked, [this](){
      const auto& tileTexture = m_mapEditorCanvas->getTileTexture();
      if(tileTexture.getSize().x > 0 && tileTexture.getSize().y > 0) {
        auto warningRes = QMessageBox::warning(this, "Warning", "Importing a new tileset will reset the tiles on the map! Continue?", QMessageBox::StandardButton::No, QMessageBox::StandardButton::Yes);
        if(warningRes == QMessageBox::StandardButton::Yes) {
          openTileFileBrowser();
        }
      } else {
        openTileFileBrowser();
      }
    });

    m_layout->addWidget(m_loadTilesetButton, 2, 0, 1, 1);
    m_layout->addWidget(m_importTilesetButton, 3, 0, 1, 1);
  }

  void TilesetToolbox::openTileFileBrowser()
  {
    auto tileFilename = QFileDialog::getOpenFileName(nullptr,
                                                     tr("Open Image"),
                                                     "",
                                                     tr("Image Files (*.png *.jpg *.jpeg)"));
    if(tileFilename.size() > 0) {
      if(m_mapEditorCanvas->setTileTexture(tileFilename.toUtf8().data())) {
        resetMapTiles();
        setTileSelection();
      } else {
        QMessageBox::critical(this,
                              "Error",
                              "Failed to load image! Image type is unknown.",
                              QMessageBox::Ok);
      }
    }
  }

  void TilesetToolbox::setTileSelection()
  {
    m_tileList->clear();
    m_tileRectList.clear();

    auto tileSetImage = m_mapEditorCanvas->getTileTexture().copyToImage();
    unsigned int tileCount = 0;
    for(unsigned int j = 0; j < tileSetImage.getSize().y; j += m_tileSize) {
      for(unsigned int i = 0; i < tileSetImage.getSize().x; i += m_tileSize) {
        addImageToTileSelection(copyImageRectToImage(tileSetImage, sf::FloatRect(i, j, m_tileSize, m_tileSize)),
                                tileCount,
                                tileCount);
        m_tileRectList.push_back(sf::FloatRect(i, j, m_tileSize, m_tileSize));
        ++tileCount;
      }
    }
  }

  void TilesetToolbox::resetMapTiles()
  {
    m_mapEditorCanvas->resetTileTextureSizes(strat::TILE_SIZE, strat::TILE_SIZE);
  }

  void TilesetToolbox::addImageToTileSelection(const sf::Image& image,
                                               unsigned int tileCount,
                                               std::size_t index)
  {
    return addImageToTileSelection(image,
                                   gengine2d::string_t("TILE_TEXTURE_" + getTileCountString(tileCount)),
                                   index);
  }

  void TilesetToolbox::addImageToTileSelection(const sf::Image& image,
                                               const gengine2d::string_t& tileName,
                                               std::size_t index)
  {
    auto tileImageIcon = qtImageToQtIcon(sfImageToQtImage(image));
    QListWidgetItem* item = new QListWidgetItem(tileImageIcon,
                                                tileName.c_str(),
                                                m_tileList);
    item->setData(Qt::UserRole, QVariant(static_cast<int>(index)));
    m_tileList->addItem(item);
  }
}
