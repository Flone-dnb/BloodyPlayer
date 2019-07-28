#pragma once

// Qt
#include <QScrollArea>

// STL
#include <vector>



namespace Ui
{
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

    // Sends dropped and filtered files to MainWindow
    void signalDrop(QStringList filteredPaths);

public:

    explicit TrackList(QWidget *parent = nullptr);


    // Searches for tracks in the given directory
    void addDirectory(QString path);


    ~TrackList();

protected:

    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

private:

    QStringList filterPaths(QStringList paths);



    WaitWindow* pWaitWindow;
    Ui::TrackList *ui;
};
