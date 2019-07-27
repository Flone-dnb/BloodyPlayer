#pragma once

#include <QWidget>

class QMouseEvent;

namespace Ui {
class TrackWidget;
}

class TrackWidget : public QWidget
{
    Q_OBJECT

signals:

    void signalSetNumber(size_t iNumber);

    void signalDoubleClick(size_t trackIndex);

    void signalSetPlaying();

    void signalDisablePlaying();

    void signalSelected(size_t iTrackIndex);

public:

    explicit TrackWidget(QString TrackName, QString TrackInfo, QString TrackTime, QWidget *parent = nullptr);

    void setPlaing();
    void setNumber(size_t iNumber);
    void disablePlaying();
    void disableSelected();

    ~TrackWidget();



    QString trackName;
    QString trackInfo;
    QString trackTime;
    size_t trackIndex;

protected:

    void mouseDoubleClickEvent(QMouseEvent* ev);

    void mousePressEvent(QMouseEvent* ev);

private slots:

    void slotSetNumber(size_t iNumber);

    void slotSetPlaying();

    void slotDisablePlaying();

private:

    Ui::TrackWidget *ui;
    bool bPlaying;
    bool bSelected;
};
