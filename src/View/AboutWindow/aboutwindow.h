// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#pragma once


#include <QMainWindow>


class QCloseEvent;

namespace Ui
{
    class AboutWindow;
}


// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------


class AboutWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit AboutWindow(QString sBloodyVersion, QWidget *parent = nullptr);

    ~AboutWindow();

protected:

    void closeEvent (QCloseEvent* pEvent);

private slots:
    void on_pushButton_clicked();

private:
    Ui::AboutWindow *ui;
};
