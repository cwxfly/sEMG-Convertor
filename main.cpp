#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
 //   a.seWindowIcon(QIcon("fish.ico"));
    Dialog w;
    w.show();
    return a.exec();
}
