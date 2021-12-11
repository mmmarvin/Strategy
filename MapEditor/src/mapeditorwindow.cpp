#include <iostream>
#include <SFML/Config.hpp>
#include <QButtonGroup>
#include <QDockWidget>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QStatusBar>
#include "../Game/src/core/debug/debug_log.h"
#include "../Game/src/game/game_config.h"
#include "map_editor_common.h"
#include "mapeditorcanvas.h"
#include "mapeditorwindow.h"

namespace strat_me
{
  static constexpr auto INITIAL_MAP_WINDOW_WIDTH = 800;
  static constexpr auto INITIAL_MAP_WINDOW_HEIGHT = 600;

  MapEditorWindow::MapEditorWindow() :
    m_mapFilename("")
  {
    updateWindowTitle();

    createCanvas();
    createMenus();
    createToolbars();
    createDocks();

    statusBar()->showMessage("[0, 0][0, 0]");
  }

  void MapEditorWindow::createMenus()
  {
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_editMenu = menuBar()->addMenu(tr("&Edit"));
    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_helpMenu = menuBar()->addMenu(tr("&Help"));

    createFileSubMenus();
    createViewSubMenus();
    createHelpSubMenus();
  }

  void MapEditorWindow::createCanvas()
  {
    m_canvas = new MapEditorCanvas(this,
                                   QSize(INITIAL_MAP_WINDOW_WIDTH,
                                         INITIAL_MAP_WINDOW_HEIGHT));
    this->setCentralWidget(m_canvas);
  }

  void MapEditorWindow::createToolbars()
  {
    m_toolBar = new QToolBar(tr("Main Toolbar"), this);
    this->addToolBar(Qt::ToolBarArea::TopToolBarArea, m_toolBar);
    m_toolBarButtons = new QButtonGroup(m_toolBar);
    m_toolBarButtons->setExclusive(true);

    m_selectToolBarButton = new QPushButton(m_toolBar);
    m_selectToolBarButton->setToolTip("Selection Mode");
    m_selectToolBarButton->setCheckable(true);
    m_selectToolBarButton->setChecked(true);
    m_selectToolBarButton->setIcon(QIcon("res/select_icon.png"));
    m_toolBar->addWidget(m_selectToolBarButton);
    m_toolBarButtons->addButton(m_selectToolBarButton);
    connect(m_selectToolBarButton, &QPushButton::clicked, [this]() {
      m_canvas->setMode(MapEditorCanvas::EMode::MODE_SELECT);
    });

    m_moveToolBarButton = new QPushButton(m_toolBar);
    m_moveToolBarButton->setToolTip("Move Mode");
    m_moveToolBarButton->setCheckable(true);
    m_moveToolBarButton->setIcon(QIcon("res/move_icon.png"));
    m_toolBar->addWidget(m_moveToolBarButton);
    m_toolBarButtons->addButton(m_moveToolBarButton);
    connect(m_moveToolBarButton, &QPushButton::clicked, [this]() {
      m_canvas->setMode(MapEditorCanvas::EMode::MODE_MOVE);
    });

    m_tileToolBarButton = new QPushButton(m_toolBar);
    m_tileToolBarButton->setToolTip("Tile Mode");
    m_tileToolBarButton->setCheckable(true);
    m_tileToolBarButton->setIcon(QIcon("res/tile_icon.png"));
    m_toolBar->addWidget(m_tileToolBarButton);
    m_toolBarButtons->addButton(m_tileToolBarButton);
    connect(m_tileToolBarButton, &QPushButton::clicked, [this]() {
      m_canvas->setMode(MapEditorCanvas::EMode::MODE_TILE);
    });

    m_objectToolBarButton = new QPushButton(m_toolBar);
    m_objectToolBarButton->setToolTip("Object Mode");
    m_objectToolBarButton->setCheckable(true);
    m_objectToolBarButton->setIcon(QIcon("res/object_icon.png"));
    m_toolBar->addWidget(m_objectToolBarButton);
    m_toolBarButtons->addButton(m_objectToolBarButton);
    connect(m_objectToolBarButton, &QPushButton::clicked, [this]() {
      m_canvas->setMode(MapEditorCanvas::EMode::MODE_OBJECT);
    });
  }

  void MapEditorWindow::createDocks()
  {
    auto* dock = new QDockWidget(tr("Tileset Toolbox"), this);
    m_tilesetToolbox = new TilesetToolbox(dock, m_canvas);

    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(m_tilesetToolbox);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    dock = new QDockWidget(tr("Object Toolbox"), this);
    m_objectToolbox = new ObjectToolbox(dock, m_canvas);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(m_objectToolbox);
    addDockWidget(Qt::RightDockWidgetArea, dock);
  }

  void MapEditorWindow::createFileSubMenus()
  {
    m_fileNewFile = new QAction(tr("&New..."), this);
    m_fileOpenFile = new QAction(tr("&Open..."), this);
    m_fileSaveFile = new QAction(tr("&Save..."), this);
    m_fileSaveAsFile = new QAction(tr("&Save As..."), this);

    m_fileMenu->addAction(m_fileNewFile);
    m_fileMenu->addAction(m_fileOpenFile);
    m_fileMenu->addAction(m_fileSaveFile);
    m_fileMenu->addAction(m_fileSaveAsFile);

    connect(m_fileOpenFile, &QAction::triggered, [this] {
      auto chosenFilename = QFileDialog::getOpenFileName(nullptr,
                                                         tr("Open Map"),
                                                         "",
                                                         tr((strat::GAME_TITLE + " Map Files (*.smap)").c_str()));

      if(chosenFilename.size() > 0) {
        if(!m_canvas->loadFromFile(chosenFilename.toUtf8().data())) {
          QMessageBox::critical(nullptr,
                                "Error",
                                "Cannot open map file \"" + chosenFilename + "\"",
                                QMessageBox::StandardButton::Ok);
        }
      }
    });

    connect(m_fileSaveFile, &QAction::triggered, [this] {
      if(m_mapFilename.empty()) {
        saveAsToFile();
      } else {
        saveToFile(m_mapFilename);
      }
    });

    connect(m_fileSaveAsFile, &QAction::triggered, [this] {
      saveAsToFile();
    });
  }

  void MapEditorWindow::createViewSubMenus()
  {
    m_viewGridLines = new QAction(tr("Show &Grid Lines"));
    m_viewGridLines->setCheckable(true);
    if(m_canvas->gridLines()) {
      m_viewGridLines->setChecked(true);
    } else {
      m_viewGridLines->setChecked(false);
    }
    connect(m_viewGridLines, &QAction::triggered, [this](){
      if(m_canvas->gridLines()) {
        m_canvas->showGridLines(false);
        m_viewGridLines->setChecked(false);
      } else {
        m_canvas->showGridLines(true);
        m_viewGridLines->setChecked(true);
      }
    });
    m_viewIsometric = new QAction(tr("Show &Isometric"), this);
    m_viewIsometric->setCheckable(true);
    if(m_canvas->isometric()) {
      m_viewIsometric->setChecked(true);
    } else {
      m_viewIsometric->setChecked(false);
    }
    connect(m_viewIsometric, &QAction::triggered, [this](){
      if(m_canvas->isometric()) {
        m_canvas->setIsometric(false);
      } else {
        m_canvas->setIsometric(true);
      }
    });

    m_viewTilesetToolbox = new QAction(tr("Show &Tileset Toolbox"), this);
    // FIXME: Make tileset visible when clicked
    connect(m_viewTilesetToolbox, &QAction::triggered, [this](){
//      if(!m_tilesetToolbox->isVisible()) {
        m_tilesetToolbox->show();
//      }
    });

    m_viewObjectToolbox = new QAction(tr("Show &Object Toolbox"), this);

    m_viewMenu->addAction(m_viewGridLines);
    m_viewMenu->addAction(m_viewIsometric);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_viewTilesetToolbox);
    m_viewMenu->addAction(m_viewObjectToolbox);
  }



  void MapEditorWindow::createHelpSubMenus()
  {
    m_helpAbout = new QAction(tr("&About..."), this);
    connect(m_helpAbout, &QAction::triggered, [this]() {
      gengine2d::string_t aboutString = getMapEditorName() + "\n" +
                                        getMapEditorVersion() + "\n" +
                                        "Copyright (c) " +
                                        getMapEditorCopyrightYears() + "\n\n" +
                                        "Based on Qt v.5.13.2\n" +
                                        "Based on SFML v.2.51\n\n" +
                                        strat::GAME_TITLE + " and it's Map Editor was written by\n" +
                                        "Marvin Manese";

      QMessageBox::about(this, "About", aboutString.c_str());
    });

    m_helpMenu->addAction(m_helpAbout);
  }

  void MapEditorWindow::saveToFile(const gengine2d::string_t& filename)
  {
    if(!m_canvas->saveToFile(filename)) {
      QMessageBox::critical(nullptr, "Error", "Cannot save map to file!", QMessageBox::StandardButton::Ok);
    } else {
      m_mapFilename = filename;
      updateWindowTitle();
    }
  }

  void MapEditorWindow::saveAsToFile()
  {
    auto chosenFilename = QFileDialog::getSaveFileName(nullptr,
                                                       tr("Save"),
                                                       "",
                                                       tr((strat::GAME_TITLE + " Map Files (*.smap)").c_str()));
    if(chosenFilename.size() > 0) {
       saveToFile(chosenFilename.toUtf8().data());
    }
  }

  void MapEditorWindow::updateWindowTitle()
  {
    if(m_mapFilename.empty()) {
      this->setWindowTitle(getMapEditorTitle("Untitled").c_str());
    } else {
      this->setWindowTitle(getMapEditorTitle(m_mapFilename).c_str());
    }
  }
}
