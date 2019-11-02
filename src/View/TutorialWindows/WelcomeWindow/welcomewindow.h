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
