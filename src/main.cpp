#include "../src/View/MainWindow/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    if (w.isSystemReady())
    {
        // Hide maximize button
        Qt::WindowFlags flags = w.windowFlags();
        flags &= ~Qt::WindowMaximizeButtonHint;
        w.setWindowFlags(flags);

        w.show();

        if (argc > 1)
        {
            // Load tracks

            QStringList paths;
            for (int i = 1; i < argc; i++)
            {
                paths.push_back(a.arguments().at(i));
            }

            w.slotDrop(paths);
        }

        return a.exec();
    }
    else
    {
        return 1;
    }
}
