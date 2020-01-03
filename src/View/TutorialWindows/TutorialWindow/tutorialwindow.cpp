// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#include "tutorialwindow.h"
#include "ui_tutorialwindow.h"

#include <QMessageBox>

TutorialWindow::TutorialWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TutorialWindow)
{
    ui->setupUi(this);

    setFixedSize (width(), height());

    cCurrentSlide = 0;

    changeSlide();
}



void TutorialWindow::on_pushButton_next_clicked()
{
    cCurrentSlide++;


    if (cCurrentSlide > 0)
    {
        ui->pushButton_prev->setEnabled(true);
    }


    changeSlide();
}


void TutorialWindow::on_pushButton_prev_clicked()
{
    cCurrentSlide--;


    if (cCurrentSlide == 0)
    {
        ui->pushButton_prev->setEnabled(false);
    }

    if (cCurrentSlide < 3)
    {
        ui->pushButton_next->setText("Next");
    }


    changeSlide();
}

void TutorialWindow::changeSlide()
{
    switch (cCurrentSlide)
    {
    case(0):

        ui->label_title->setText("Drag'n'Drop");
        ui->label_desc->setText ("Select file(-s) or folder(-s) in Explorer and move them here to add music.");
        ui->label_image->setPixmap(QPixmap(":/dragndrop_slide.png").scaled( width() - 50, height() * 0.8f, Qt::AspectRatioMode::KeepAspectRatio ));

        break;

    case(1):

        ui->label_title->setText("Search");
        ui->label_desc->setText ("Use Ctrl + F to open the search window to search for the desired track in the tracklist.");
        ui->label_image->setPixmap(QPixmap(":/find_slide.png").scaled( width() - 50, height() * 0.8f, Qt::AspectRatioMode::KeepAspectRatio ));

        break;

    case(2):

        ui->label_title->setText("Repeat section");
        ui->label_desc->setText ("Click the right mouse button on the oscillogram to set the left bound for the repetition, click the right mouse "
                                 "button again to set the right bound and make a repetition section in which music will repeat.");
        ui->label_image->setPixmap(QPixmap(":/repeat_slide.png").scaled( width() - 50, height() * 0.8f, Qt::AspectRatioMode::KeepAspectRatio ));

        break;

    case(3):

        ui->label_title->setText("Tracklist management");
        ui->label_desc->setText ("Move tracks in the tracklist or delete some of them by right-clicking on the track or using hotkeys.");
        ui->label_image->setPixmap(QPixmap(":/context_slide.png").scaled( width() - 50, height() * 0.8f, Qt::AspectRatioMode::KeepAspectRatio ));

        ui->pushButton_next->setText("End");

        break;

    case(4):

        emit end();
        close();

        break;
    }
}



void TutorialWindow::closeEvent(QCloseEvent *ev)
{
    emit close();
    deleteLater();
}



TutorialWindow::~TutorialWindow()
{
    delete ui;
}
