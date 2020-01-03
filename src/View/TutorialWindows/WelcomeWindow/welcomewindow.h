// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#pragma once

#include <QMainWindow>


class QCloseEvent;

namespace Ui
{
    class WelcomeWindow;
}

class WelcomeWindow : public QMainWindow
{
    Q_OBJECT

signals:

    void signalDoNotShowTutorialAgain();
    void signalEndTutorial();

public:

    explicit WelcomeWindow(QWidget *parent = nullptr);

    ~WelcomeWindow();

public slots:

    void tutorialClosed();
    void tutorialEnd();

protected:

    void closeEvent(QCloseEvent* ev);

private slots:

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:

    Ui::WelcomeWindow *ui;
};
