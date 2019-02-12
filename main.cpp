#include "widget.h"
#include <QApplication>
#include "dialoglist.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DialogList w;
    w.show();

    return a.exec();
}
