// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

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


    Ui::TrackList *ui;
};
