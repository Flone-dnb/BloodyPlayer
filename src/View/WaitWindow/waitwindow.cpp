// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#include "waitwindow.h"
#include "ui_waitwindow.h"

WaitWindow::WaitWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WaitWindow)
{
    ui->setupUi(this);
    setFixedSize(width(), height());
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

void WaitWindow::setLabelText(QString text)
{
    ui->label->setText(text);
}

void WaitWindow::setProgressValue(int value)
{
    ui->progressBar->setValue(value);
}



WaitWindow::~WaitWindow()
{
    delete ui;
}
