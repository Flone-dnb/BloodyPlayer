#include "vstwindow.h"
#include "ui_vstwindow.h"

#include <QCloseEvent>

VSTWindow::VSTWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VSTWindow)
{
    ui->setupUi(this);

    // Hide maximize & minimize buttons
    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowMaximizeButtonHint;
    flags &= ~Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags);

    setCentralWidget(ui->widget);
}

HWND VSTWindow::getVSTWidgetHandle()
{
    return reinterpret_cast<HWND>(ui->widget->winId());
}



void VSTWindow::closeEvent(QCloseEvent *ev)
{
    emit updateAudio();
    hide();
    ev->ignore();
}

void VSTWindow::on_actionUnload_triggered()
{
    hide();
    emit unloadVST();
}


VSTWindow::~VSTWindow()
{
    delete ui;
}
