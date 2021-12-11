#include <QApplication>
#include "src/mapeditorwindow.h"

int main(int argc, char** argv)
{
  QApplication a(argc, argv);
  strat_me::MapEditorWindow w;
  w.show();

  return a.exec();
}
