﻿// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

#include "tracklist.h"
#include "ui_tracklist.h"

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
}

void TrackList::addDirectory(QString path)
{
    QStringList list;
    list.push_back(path);

    QStringList filteredPaths = filterPaths(list);

    if (filteredPaths.size() > 0)
    {
        emit signalDrop(filteredPaths);
    }
}

void TrackList::dragEnterEvent(QDragEnterEvent *event)
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

        bool bFolderFound = false;

        // Look for extensions
        for (int i = 0; i < pathList.size(); i++)
        {
            // Determine the start position of the extension.
            int iExtensionPos = -1;
            for (int j = pathList[i].size() - 1; j >=0; j--)
            {
                if (pathList[i][j] == '.')
                {
                    iExtensionPos = j + 1;
                    break;
                }
            }

            if (iExtensionPos != -1)
            {
                // File

                // Read the extension
                QString ext = "";
                for (int j = iExtensionPos; j < pathList[i].size(); j++)
                {
                    ext += pathList[i][j];
                }

                ext = ext.toUpper();
                if ( (ext == "MP3") || (ext == "FLAC") || (ext == "WAV") || (ext == "OGG") )
                {
                    filteredPaths.push_back(pathList[i]);
                }
            }
            else
            {
                // Folder (maybe)
                bFolderFound = true;
            }
        }

        if ( (filteredPaths.size() > 0) || bFolderFound)
        {
            event->acceptProposedAction();
        }
    }
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

        filteredPaths = filterPaths(pathList);

        if (filteredPaths.size() > 0)
        {
            emit signalDrop(filteredPaths);
        }
    }
}

QStringList TrackList::filterPaths(QStringList paths)
{
    QStringList filteredPaths;

    // Look for extensions
    for (int i = 0; i < paths.size(); i++)
    {
        // Determine the start position of the extension.
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
            // File

            // Read the extension
            QString ext = "";
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
    delete ui;
}
