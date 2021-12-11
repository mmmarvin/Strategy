#ifndef OBJECTTOOLBOX_H
#define OBJECTTOOLBOX_H

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QWidget>

namespace strat_me
{
  class MapEditorCanvas;
  class ObjectToolbox : public QWidget
  {
  public:
    ObjectToolbox(QWidget* parent, MapEditorCanvas* mapEditorCanvas);

  private:
    void addObjectNameWidgets();
    void addObjectPropertyWidget();
    void addCustomObjectNameWidgets();

    QLabel*               m_objectName;
    QComboBox*            m_objectNameSelection;

    QTableWidget*         m_customObjectPropertiesView;

    QLabel*               m_customObjectNameLabel;
    QTextEdit*            m_customObjectNameEdit;
    QPushButton*          m_customObjectNameButton;

    MapEditorCanvas*      m_mapEditorCanvas;
  };
}

#endif // OBJECTTOOLBOX_H
