#pragma once

#include <QWidget>

class QMouseEvent;
class QMenu;
class QAction;

namespace Ui {
class TrackWidget;
}

class TrackWidget : public QWidget
{
    Q_OBJECT

signals:

    // Context menu signals
    void signalDelete();
    void signalMoveUp();
    void signalMoveDown();

    void signalDoubleClick(size_t trackIndex);
    void signalSetPlaying();
    void signalDisablePlaying();
    void signalSelected(size_t iTrackIndex);

public:

    explicit TrackWidget(QString TrackName, QString TrackInfo, QString TrackTime, QWidget *parent = nullptr);

    void setPlaying();
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

    // Context menu
    void slotMoveUp();
    void slotMoveDown();
    void slotDelete();

    void slotSetPlaying();
    void slotDisablePlaying();

    void on_TrackWidget_customContextMenuRequested(const QPoint &pos);

private:

    Ui::TrackWidget *ui;

    // Context menu
    QMenu* pMenuContextMenu;
        QAction* pActionMoveUp;
        QAction* pActionMoveDown;
        QAction* pActionDelete;

    QString styleDefault;
    QString styleSelected;
    QString stylePlaying;

    bool bPlaying;
    bool bSelected;
};
