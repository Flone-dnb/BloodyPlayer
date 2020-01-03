// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

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
    emit unloadVST();
}


VSTWindow::~VSTWindow()
{
    delete ui;
}
