// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#include "../src/View/MainWindow/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    if (w.isSystemReady())
    {
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
