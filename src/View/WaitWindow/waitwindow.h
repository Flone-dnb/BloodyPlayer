// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#pragma once

#include <QMainWindow>

namespace Ui {
class WaitWindow;
}

class WaitWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit WaitWindow(QWidget *parent = nullptr);

    void setLabelText(QString text);
    void setProgressValue(int value);

    ~WaitWindow();


private:

    Ui::WaitWindow *ui;
};
