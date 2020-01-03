// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#include "aboutwindow.h"
#include "ui_aboutwindow.h"

// Qt
#include <QCloseEvent>
#include <QDesktopServices>
#include <QUrl>

AboutWindow::AboutWindow(QString sBloodyVersion, QWidget *parent) : QMainWindow(parent), ui(new Ui::AboutWindow)
{
    ui->setupUi(this);

    setFixedSize ( width (), height () );


    ui ->label_appIcon    ->setPixmap ( QPixmap(":/bloodyLogo2.png").scaled (128, 128, Qt::KeepAspectRatio) );
    ui ->label_version    ->setText   ( "Bloody Player. Version: " + sBloodyVersion + "." );
    ui ->label_copyright  ->setText   ( "Copyright (c) 2019.\nAleksandr \"Flone\" Tretyakov." );
}



void AboutWindow::closeEvent(QCloseEvent* pEvent)
{
    deleteLater();
}



void AboutWindow::on_pushButton_clicked()
{
    QDesktopServices::openUrl (QUrl("https://github.com/Flone-dnb/BloodyPlayer"));
}


AboutWindow::~AboutWindow()
{
    delete ui;
}
