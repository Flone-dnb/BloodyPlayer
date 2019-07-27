#include "tracklist.h"
#include "ui_tracklist.h"

#include "../src/View/WaitWindow/waitwindow.h"

#include <QDropEvent>
#include <QDir>
#include <QMimeData>
#include <QKeyEvent>

TrackList::TrackList(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::TrackList)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    pWaitWindow = new WaitWindow(this);
    pWaitWindow->setWindowModality(Qt::WindowModality::WindowModal);
}

void TrackList::addDirectory(QString path)
{
    QStringList list;
    list.push_back(path);

    pWaitWindow->show();
    QStringList filteredPaths = filterPaths(list);

    if (filteredPaths.size() > 0)
    {
        emit signalDrop(filteredPaths);
    }

    pWaitWindow->hide();
}

void TrackList::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void TrackList::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        for (int i = 0; i < urlList.size(); i++)
        {
            pathList.append(urlList.at(i).toLocalFile());
        }

        QStringList filteredPaths;

        pWaitWindow->show();
        filteredPaths = filterPaths(pathList);

        if (filteredPaths.size() > 0)
        {
            emit signalDrop(filteredPaths);
        }

        pWaitWindow->hide();
    }
}

void TrackList::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Delete)
    {
        emit signalDeleteTrack();
    }
}

QStringList TrackList::filterPaths(QStringList paths)
{
    QApplication::processEvents();

    QStringList filteredPaths;

    // Look for extensions
    for (int i = 0; i < paths.size(); i++)
    {
        int iExtensionPos = -1;

        for (int j = paths[i].size() - 1; j >=0; j--)
        {
            if (paths[i][j] == '.')
            {
                iExtensionPos = j + 1;
                break;
            }
        }

        if (iExtensionPos != -1)
        {
            QString ext = "";

            // File
            for (int j = iExtensionPos; j < paths[i].size(); j++)
            {
                ext += paths[i][j];
            }

            ext = ext.toUpper();
            if ( (ext == "MP3") || (ext == "FLAC") || (ext == "WAV") || (ext == "OGG") )
            {
                filteredPaths.push_back(paths[i]);
            }
        }
        else
        {
            // Folder
            QDir folder(paths[i]);

            QStringList in = folder.entryList( QDir::Dirs | QDir::Files );
            for (int j = 0; j < in.size(); j++)
            {
                QString fullPath = folder.absolutePath() + "/";
                in[j] = fullPath + in[j];
            }


            QStringList files = filterPaths( in );
            for (int j = 0; j < files.size(); j++)
            {
                filteredPaths.push_back(files[j]);
            }
        }
    }

    return filteredPaths;
}

TrackList::~TrackList()
{
    delete pWaitWindow;
    delete ui;
}
