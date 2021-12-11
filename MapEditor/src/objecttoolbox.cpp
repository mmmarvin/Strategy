#include "../Game/src/core/utility/general_utility.h"
#include "objecttoolbox.h"

namespace strat_me
{
  ObjectToolbox::ObjectToolbox(QWidget* parent, MapEditorCanvas* mapEditorCanvas) :
    QWidget(parent),
    m_mapEditorCanvas(mapEditorCanvas)
  {
    auto* layout = new QGridLayout(this);
    auto* objectNameLayout = new QGridLayout;
    auto customObjectLayout = new QGridLayout;

    layout->addLayout(objectNameLayout, 0, 0, 1, 1);
    layout->addLayout(customObjectLayout, 2, 0, 1, 1);

    addObjectNameWidgets();

    objectNameLayout->addWidget(m_objectName, 0, 0, 1, 1);
    objectNameLayout->addWidget(m_objectNameSelection, 0, 1, 1, 5);

    addObjectPropertyWidget();
    layout->addWidget(m_customObjectPropertiesView, 1, 0, 1, 1);

    addCustomObjectNameWidgets();

    customObjectLayout->addWidget(m_customObjectNameLabel, 0, 0, 1, 1);
    customObjectLayout->addWidget(m_customObjectNameEdit, 0, 1, 1, 1);
    customObjectLayout->addWidget(m_customObjectNameButton, 0, 2, 1, 1);
  }

  void ObjectToolbox::addObjectNameWidgets()
  {
    m_objectName = new QLabel("Object: ", this);
    m_objectNameSelection = new QComboBox(this);
  }

  void ObjectToolbox::addObjectPropertyWidget()
  {
    m_customObjectPropertiesView = new QTableWidget(this);
  }

  void ObjectToolbox::addCustomObjectNameWidgets()
  {
    m_customObjectNameLabel = new QLabel("Custom Name:", this);
    m_customObjectNameEdit = new QTextEdit(this);
    m_customObjectNameEdit->setFixedHeight(gengine2d::general_utils::max(m_customObjectNameEdit->font().pixelSize(),
                                                                         m_customObjectNameEdit->font().pointSize()) * 3);
    m_customObjectNameButton = new QPushButton("Add", this);
  }
}
