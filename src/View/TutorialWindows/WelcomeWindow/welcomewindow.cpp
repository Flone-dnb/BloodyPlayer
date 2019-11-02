#include "welcomewindow.h"
#include "ui_welcomewindow.h"

// Qt
#include <QCloseEvent>

// Custom
#include "../src/View/TutorialWindows/TutorialWindow/tutorialwindow.h"

WelcomeWindow::WelcomeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WelcomeWindow)
{
    ui->setupUi(this);

    setFixedSize (width(), height());
}




void WelcomeWindow::closeEvent(QCloseEvent *ev)
{
    deleteLater();
}


void WelcomeWindow::on_pushButton_clicked()
{
    if (ui->checkBox->isChecked())
    {
        emit signalDoNotShowTutorialAgain();
    }

    close();
}


void WelcomeWindow::on_pushButton_2_clicked()
{
    TutorialWindow* pTutorialWindow = new TutorialWindow(this);

    connect(pTutorialWindow, &TutorialWindow::close, this, &WelcomeWindow::tutorialClosed);
    connect(pTutorialWindow, &TutorialWindow::end, this, &WelcomeWindow::tutorialEnd);

    hide();
    pTutorialWindow->show();
}

void WelcomeWindow::tutorialClosed()
{
    close();
}

void WelcomeWindow::tutorialEnd()
{
    emit signalEndTutorial();
    close();
}

WelcomeWindow::~WelcomeWindow()
{
    delete ui;
}

