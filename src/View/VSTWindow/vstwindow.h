// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#pragma once

#include <QMainWindow>

namespace Ui {
class VSTWindow;
}

class QCloseEvent;

class VSTWindow : public QMainWindow
{
    Q_OBJECT

signals:

    void unloadVST();
    void updateAudio();

public:

    explicit VSTWindow(QWidget *parent = nullptr);

#if _WIN32
    HWND getVSTWidgetHandle();
#endif

    ~VSTWindow();

protected:

    void closeEvent(QCloseEvent* ev);

private slots:

    void on_actionUnload_triggered();

private:

    Ui::VSTWindow *ui;
};
