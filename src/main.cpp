#include "../src/View/MainWindow/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Qt::WindowFlags flags = w.windowFlags();
    flags &= ~Qt::WindowMaximizeButtonHint;
    w.setWindowFlags(flags);
    w.show();

    return a.exec();
}
