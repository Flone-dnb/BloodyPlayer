#include "trackwidget.h"
#include "ui_trackwidget.h"

#include <QMouseEvent>

TrackWidget::TrackWidget(QString TrackName, QString TrackInfo, QString TrackTime, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackWidget)
{
    ui->setupUi(this);
    setFixedHeight(height());

    connect(this, &TrackWidget::signalSetPlaying, this, &TrackWidget::slotSetPlaying);
    connect(this, &TrackWidget::signalDisablePlaying, this, &TrackWidget::slotDisablePlaying);

    ui->label_TrackName->setText(TrackName);
    ui->label_TrackInfo->setText(TrackInfo);
    ui->label_TrackTime->setText(TrackTime);

    trackName = TrackName;
    trackInfo = TrackInfo;
    trackTime = TrackTime;

    bPlaying = false;
    bSelected = false;
}

void TrackWidget::setPlaying()
{
    emit signalSetPlaying();
}

void TrackWidget::setNumber(size_t iNumber)
{
     ui->label_No->setText( QString::number(iNumber) );

     trackIndex = iNumber - 1;
}

void TrackWidget::disablePlaying()
{
    emit signalDisablePlaying();
}

void TrackWidget::disableSelected()
{
    bSelected = false;

    if (bPlaying)
    {
        slotSetPlaying();
    }
    else
    {
        disablePlaying();
    }
}

void TrackWidget::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if (bSelected)
    {
        bSelected = false;
        emit signalSelected(trackIndex);
    }

    emit signalDoubleClick(trackIndex);
    setPlaying();
}

void TrackWidget::mousePressEvent(QMouseEvent *ev)
{
    if (bSelected)
    {
        if (bPlaying)
        {
            ui->frame->setStyleSheet("QFrame"
                                     "{"
                                     "  background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(58, 18, 18, 255), stop:1 rgba(50, 32, 21, 255));"
                                     "  border: 1px solid darkred;"
                                        "color: white;"
                                     "}"
                                     ""
                                     "QFrame:hover"
                                     "{"
                                     "  background-color: rgb(78, 78, 78);"
                                     "}");
        }
        else
        {
            ui->frame->setStyleSheet("QFrame"
                                     "{"
                                     "  background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(44, 44, 44, 255));"
                                     "  border: 1px solid darkred;"
                                        "color: white;"
                                     "}"
                                     ""
                                     "QFrame:hover"
                                     "{"
                                     "  background-color: rgb(78, 78, 78);"
                                     "}");
        }

        emit signalSelected(trackIndex);
        bSelected = false;
    }
    else
    {
        ui->frame->setStyleSheet("QFrame"
                                 "{"
                                 "  background-color: rgb(77, 38, 38);"
                                 "  border: 1px solid darkred;"
                                    "color: white;"
                                 "}"
                                 ""
                                 "QFrame:hover"
                                 "{"
                                 "  background-color: rgb(78, 78, 78);"
                                 "}");

        emit signalSelected(trackIndex);
        bSelected = true;
    }
}

void TrackWidget::slotSetPlaying()
{
    if (!bSelected)
    {
        ui->frame->setStyleSheet("QFrame"
                                 "{"
                                 "  background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(58, 18, 18, 255), stop:1 rgba(50, 32, 21, 255));"
                                 "  border: 1px solid darkred;"
                                    "color: white;"
                                 "}"
                                 ""
                                 "QFrame:hover"
                                 "{"
                                 "  background-color: rgb(78, 78, 78);"
                                 "}");
        update();
    }

    bPlaying = true;
}

void TrackWidget::slotDisablePlaying()
{
    if (bSelected)
    {
        ui->frame->setStyleSheet("QFrame"
                                 "{"
                                 "  background-color: rgb(77, 38, 38);"
                                 "  border: 1px solid darkred;"
                                    "color: white;"
                                 "}"
                                 ""
                                 "QFrame:hover"
                                 "{"
                                 "  background-color: rgb(78, 78, 78);"
                                 "}");
    }
    else
    {
        ui->frame->setStyleSheet("QFrame"
                                 "{"
                                 "  background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 rgba(40, 40, 40, 255), stop:1 rgba(44, 44, 44, 255));"
                                 "  border: 1px solid darkred;"
                                    "color: white;"
                                 "}"
                                 ""
                                 "QFrame:hover"
                                 "{"
                                 "  background-color: rgb(78, 78, 78);"
                                 "}");
    }

    bPlaying = false;
}

TrackWidget::~TrackWidget()
{
    delete ui;
}
