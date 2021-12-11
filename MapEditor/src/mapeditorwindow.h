#ifndef MAPEDITORWINDOW_H
#define MAPEDITORWINDOW_H

#include <QMainWindow>
#include <QLayout>
#include <QToolBar>
#include "../Game/src/core/type.h"
#include "objecttoolbox.h"
#include "tilesettoolbox.h"

namespace strat_me
{
  class MapEditorCanvas;
  class MapEditorWindow : public QMainWindow
  {
  public:
    MapEditorWindow();

  private:
    void createMenus();
    void createCanvas();
    void createToolbars();
    void createDocks();

    void createFileSubMenus();
    void createViewSubMenus();
    void createHelpSubMenus();

    void saveToFile(const gengine2d::string_t& filename);
    void saveAsToFile();

    void updateWindowTitle();

    QMenu*              m_fileMenu;
    QMenu*              m_editMenu;
    QMenu*              m_viewMenu;
    QMenu*              m_helpMenu;

    QAction*            m_fileNewFile;
    QAction*            m_fileOpenFile;
    QAction*            m_fileSaveFile;
    QAction*            m_fileSaveAsFile;
    QAction*            m_viewGridLines;
    QAction*            m_viewIsometric;
    QAction*            m_viewTilesetToolbox;
    QAction*            m_viewObjectToolbox;
    QAction*            m_helpAbout;

    QButtonGroup*       m_toolBarButtons;
    QPushButton*        m_selectToolBarButton;
    QPushButton*        m_moveToolBarButton;
    QPushButton*        m_tileToolBarButton;
    QPushButton*        m_objectToolBarButton;

    MapEditorCanvas*    m_canvas;
    TilesetToolbox*     m_tilesetToolbox;
    ObjectToolbox*      m_objectToolbox;
    QToolBar*           m_toolBar;

    gengine2d::string_t m_mapFilename;
  };
}

#endif // MAPEDITORWINDOW_H
