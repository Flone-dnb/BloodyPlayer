#pragma once
#include <QScrollArea>
#include <vector>

namespace Ui {
class TrackList;
}

class QDragEnterEvent;
class QDropEvent;
class WaitWindow;
class QKeyEvent;

class TrackList : public QScrollArea
{
    Q_OBJECT

signals:

    void signalDeleteTrack();

    void signalDrop(QStringList filteredPaths);

public:

    explicit TrackList(QWidget *parent = nullptr);

    void addDirectory(QString path);

    ~TrackList();

protected:

    void dragEnterEvent(QDragEnterEvent* event);

    void dropEvent(QDropEvent* event);

    void keyPressEvent(QKeyEvent* ev);

private:

    QStringList filterPaths(QStringList paths);

    WaitWindow* pWaitWindow;
    Ui::TrackList *ui;
};
