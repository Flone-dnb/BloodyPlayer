#include "mainwindow.h"
#include "ui_mainwindow.h"

// Qt
#include <QMessageBox>
#include <QFileDialog>
#include <QKeyEvent>
#include <QScrollBar>

// Custom
#include "../src/Controller/controller.h"
#include "../src/View/TrackWidget/trackwidget.h"
#include "../src/globalparams.h"
#include "../src/View/WaitWindow/waitwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->verticalLayout_Tracks->setAlignment( Qt::AlignTop );
    ui->horizontalSlider->setValue(static_cast<int>(DEFAULT_VOLUME*100));
    ui->label_volume->setText("Volume: " + QString::number(DEFAULT_VOLUME*100) + "%");

    iSelectedTrackIndex = -1;
    // Will be 'false' if something will go wrong.
    bSystemReady = true;

    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<std::wstring>("std::wstring");
    qRegisterMetaType<size_t>("size_t");

    // This to this
    connect(this, &MainWindow::signalShowWaitWindow, this, &MainWindow::slotShowWaitWindow);
    connect(this, &MainWindow::signalHideWaitWindow, this, &MainWindow::slotHideWaitWindow);
    connect(this, &MainWindow::signalSetProgress,    this, &MainWindow::slotSetProgress);
    connect(this, &MainWindow::signalSetNumber,      this, &MainWindow::slotSetNumber);
    connect(this, &MainWindow::signalShowMessageBox, this, &MainWindow::slotShowMessageBox);

    // Tracklist connects
    connect(ui->scrollArea, &TrackList::signalDrop, this, &MainWindow::slotDrop);

    connect(this, &MainWindow::signalSetTrack,      this, &MainWindow::slotSetTrack);
    connect(this, &MainWindow::signalAddNewTrack,   this, &MainWindow::slotAddNewTrack);

    pController = new Controller(this);
}






void MainWindow::showMessageBox(bool errorBox, std::string text)
{
    emit signalShowMessageBox(errorBox, text);
}

void MainWindow::addNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime)
{
    emit signalAddNewTrack(trackName, trackInfo, trackTime);
}

void MainWindow::removePlayingOnTrack(size_t iTrackIndex)
{
    tracks[iTrackIndex]->disablePlaying();
}

void MainWindow::setPlayingOnTrack(size_t iTrackIndex, bool bClear)
{
    if (bClear)
    {
        emit signalSetTrack(iTrackIndex, true);
    }
    else
    {
        tracks[iTrackIndex]->setPlaying();

        ui->scrollArea->ensureWidgetVisible(tracks[iTrackIndex]);

        emit signalSetTrack(iTrackIndex);
    }
}

void MainWindow::uncheckRandomTrackButton()
{
    ui->pushButton_Random->setChecked(false);
}

void MainWindow::uncheckRepeatTrackButton()
{
    ui->pushButton_repeat->setChecked(false);
}

void MainWindow::setFocusOnTrack(size_t index)
{
    ui->scrollArea->ensureWidgetVisible(tracks[index], 50, 50);
}

void MainWindow::showWaitWindow()
{
    emit signalShowWaitWindow();
}

void MainWindow::hideWaitWindow()
{
    emit signalHideWaitWindow();
}

void MainWindow::setProgress(int value)
{
    emit signalSetProgress(value);
}

void MainWindow::removeTrack(size_t iTrackIndex)
{
    emit signalRemoveTrack(iTrackIndex);
}

void MainWindow::markAnError()
{
    // Look main.cpp
    // isSystemReady() function will return this value and app will be closed.
    bSystemReady = false;
}

bool MainWindow::isSystemReady()
{
    return bSystemReady;
}


void MainWindow::on_actionOpen_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Open file(s)", "", "Audio Files (*.mp3 *.wav *.ogg *.flac)");

    if (files.size() > 0)
    {
        std::vector<wchar_t*> localPaths;

        for (int i = 0; i < files.size(); i++)
        {
            wchar_t* pPath = new wchar_t[ static_cast<size_t>(files[i].size()) * 2 + 2 ];
            std::memset( pPath, 0, static_cast<size_t>(files[i].size()) * 2 + 2 );
            std::memcpy( pPath, files[i].toStdWString().c_str(), static_cast<size_t>(files[i].size() * 2) );

            localPaths.push_back( pPath );
        }

        pController->addTracks(localPaths);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(nullptr, "Bloody Player", "Bloody Player v1.0");
}

void MainWindow::on_pushButton_Play_clicked()
{
    if (tracks.size() > 0)
    {
        bool bPlaying;
        size_t iIndex = pController->getPlaingTrackIndex(bPlaying);

        pController->playTrack(iIndex);
    }
}

void MainWindow::on_pushButton_pause_clicked()
{
    pController->pauseTrack();
}

void MainWindow::on_pushButton_stop_clicked()
{
    pController->stopTrack();
}

void MainWindow::on_pushButton_next_clicked()
{
    pController->nextTrack();
}

void MainWindow::on_pushButton_prev_clicked()
{
    pController->prevTrack();
}

void MainWindow::slotClickedOnTrack(size_t trackIndex)
{
    pController->playTrack(trackIndex);
}

void MainWindow::slotDrop(QStringList paths)
{
    std::vector<wchar_t*> localPaths;

    for (int i = 0; i < paths.size(); i++)
    {
        wchar_t* pPath = new wchar_t[ static_cast<size_t>(paths[i].size()) * 2 + 2 ];
        std::memset( pPath, 0, static_cast<size_t>(paths[i].size()) * 2 + 2 );
        std::memcpy( pPath, paths[i].toStdWString().c_str(), static_cast<size_t>(paths[i].size() * 2) );

        localPaths.push_back( pPath );
    }

    pController->addTracks(localPaths);
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Delete)
    {
        deleteSelectedTrack();
    }
    else if (ev->key() == Qt::Key_S)
    {
        slotMoveDown();
    }
    else if (ev->key() == Qt::Key_W)
    {
        slotMoveUp();
    }
}

void MainWindow::slotShowMessageBox(bool errorBox, std::string text)
{
    if (errorBox)
    {
        QMessageBox::warning(nullptr, "Error", QString::fromStdString(text));
    }
    else
    {
        QMessageBox::information(nullptr, "Information", QString::fromStdString(text));
    }
}

void MainWindow::slotSetNumber(size_t iNumber)
{
    tracks[iNumber]->setNumber(iNumber+1);
}

void MainWindow::slotAddNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime)
{
    TrackWidget* pNewTrack = new TrackWidget( QString::fromStdWString(trackName), QString::fromStdWString(trackInfo), QString::fromStdString(trackTime) );
    connect(pNewTrack, &TrackWidget::signalDoubleClick, this, &MainWindow::slotClickedOnTrack);
    connect(pNewTrack, &TrackWidget::signalSelected,    this, &MainWindow::slotTrackSelected);
    connect(pNewTrack, &TrackWidget::signalDelete,      this, &MainWindow::deleteSelectedTrack);
    connect(pNewTrack, &TrackWidget::signalMoveUp,      this, &MainWindow::slotMoveUp);
    connect(pNewTrack, &TrackWidget::signalMoveDown,    this, &MainWindow::slotMoveDown);
    ui->verticalLayout_Tracks->addWidget(pNewTrack);

    tracks.push_back(pNewTrack);
    pNewTrack->setNumber( tracks.size() );
    pNewTrack->show();

    if (tracks.size() == 1)
    {
        ui->horizontalSlider->setEnabled(true);
    }
}

void MainWindow::slotShowWaitWindow()
{
    pWaitWindow = new WaitWindow(this);
    pWaitWindow->setWindowModality(Qt::WindowModality::ApplicationModal);
    pWaitWindow->setLabelText("Please wait.\nAdding tracks.");
    pWaitWindow->show();
}

void MainWindow::slotHideWaitWindow()
{
    pWaitWindow->close();
    delete pWaitWindow;
}
void MainWindow::slotSetProgress(int value)
{
    pWaitWindow->setProgressValue(value);
}

void MainWindow::slotSetTrack(size_t iTrackIndex, bool bClear)
{
    if (bClear)
    {
        ui->label_TrackName->setText( "Track Name" );
        ui->label_TrackInfo->setText( "Track Info" );
    }
    else
    {
        ui->label_TrackName->setText( tracks[iTrackIndex]->trackName );
        ui->label_TrackInfo->setText( tracks[iTrackIndex]->trackInfo );
    }
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    pController->changeVolume( value/100.0f );
    ui->label_volume->setText("Volume: " + QString::number(value) + "%");
}

void MainWindow::on_actionOpen_Directory_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(nullptr, "Open Directory", "");
    if (dir != "")
    {
        ui->scrollArea->addDirectory(dir);
    }
}

void MainWindow::slotTrackSelected(size_t iTrackIndex)
{
    if (iSelectedTrackIndex != static_cast<int>(iTrackIndex))
    {
        if (iSelectedTrackIndex != -1)
        {
            tracks[iSelectedTrackIndex]->disableSelected();
        }

        iSelectedTrackIndex = static_cast<int>(iTrackIndex);
    }
    else
    {
        iSelectedTrackIndex = -1;
    }
}

void MainWindow::deleteSelectedTrack()
{
    if (iSelectedTrackIndex != -1)
    {
        // 'delete' below is causing a crash if the context menu is opened on TrackWidget
        // so we use 'deleteLater()'
        //delete tracks[iSelectedTrackIndex];
        tracks[iSelectedTrackIndex]->deleteLater();
        tracks.erase(tracks.begin() + iSelectedTrackIndex);

        pController->removeTrack(iSelectedTrackIndex);

        for (size_t i = iSelectedTrackIndex; i < tracks.size(); i++)
        {
            emit signalSetNumber(i);
        }

        if (tracks.size() == 0)
        {
            ui->horizontalSlider->setValue(static_cast<int>(DEFAULT_VOLUME*100));
            ui->horizontalSlider->setEnabled(false);
        }

        iSelectedTrackIndex = -1;
    }
}

void MainWindow::slotMoveDown()
{
    if (iSelectedTrackIndex != -1)
    {
        pController->moveDown(static_cast<size_t>(iSelectedTrackIndex));

        size_t iTrackIndex = static_cast<size_t>(iSelectedTrackIndex);

        tracks[iTrackIndex]->disableSelected();

        if (iSelectedTrackIndex == (static_cast<int>(tracks.size()) - 1) )
        {
            TrackWidget* pTemp = tracks[ iTrackIndex ];
            tracks.pop_back();
            tracks.insert( tracks.begin() , pTemp);

            // ui
            QLayoutItem* pItem = ui->verticalLayout_Tracks->takeAt(iSelectedTrackIndex);
            ui->verticalLayout_Tracks->insertItem(0, pItem);

            for (size_t i = 0; i < tracks.size(); i++)
            {
                tracks[i]->setNumber(i + 1);
            }

            setFocusOnTrack(0);
        }
        else
        {
            TrackWidget* pTemp = tracks[iTrackIndex + 1];
            tracks[iTrackIndex + 1] = tracks[iTrackIndex];
            tracks[iTrackIndex] = pTemp;

            // ui
            QLayoutItem* pItem = ui->verticalLayout_Tracks->takeAt(iSelectedTrackIndex);
            ui->verticalLayout_Tracks->insertItem(iSelectedTrackIndex + 1, pItem);

            for (size_t i = iTrackIndex; i < tracks.size(); i++)
            {
                tracks[i]->setNumber(i + 1);
            }

            setFocusOnTrack(iTrackIndex);
        }

        iSelectedTrackIndex = -1;
    }
}

void MainWindow::slotMoveUp()
{
    if (iSelectedTrackIndex != -1)
    {
        pController->moveUp(static_cast<size_t>(iSelectedTrackIndex));

        size_t iTrackIndex = static_cast<size_t>(iSelectedTrackIndex);

        tracks[iTrackIndex]->disableSelected();

        if (iSelectedTrackIndex == 0 )
        {
            TrackWidget* pTemp = tracks[ iTrackIndex ];
            tracks.erase( tracks.begin() );
            tracks.push_back(pTemp);

            // ui
            QLayoutItem* pItem = ui->verticalLayout_Tracks->takeAt(iSelectedTrackIndex);
            ui->verticalLayout_Tracks->addItem(pItem);

            for (size_t i = 0; i < tracks.size(); i++)
            {
                tracks[i]->setNumber(i + 1);
            }

            setFocusOnTrack( tracks.size() - 1 );
        }
        else
        {
            TrackWidget* pTemp = tracks[iTrackIndex - 1];
            tracks[iTrackIndex - 1] = tracks[iTrackIndex];
            tracks[iTrackIndex] = pTemp;

            // ui
            QLayoutItem* pItem = ui->verticalLayout_Tracks->takeAt(iSelectedTrackIndex);
            ui->verticalLayout_Tracks->insertItem(iSelectedTrackIndex - 1, pItem);

            for (size_t i = iTrackIndex - 1; i < tracks.size(); i++)
            {
                tracks[i]->setNumber(i + 1);
            }

            setFocusOnTrack(iTrackIndex);
        }

        iSelectedTrackIndex = -1;
    }
}

void MainWindow::on_pushButton_clearPlaylist_clicked()
{
    if (tracks.size() > 0)
    {
        pController->clearPlaylist();

        for (size_t i = 0; i < tracks.size(); i++)
        {
            delete tracks[i];
        }
        tracks.clear();

        ui->label_TrackName->setText( "Track Name" );
        ui->label_TrackInfo->setText( "Track Info" );

        ui->horizontalSlider->setValue(static_cast<int>(DEFAULT_VOLUME*100));
        ui->horizontalSlider->setEnabled(false);
    }
}

void MainWindow::on_pushButton_repeat_clicked()
{
    pController->repeatTrack();
}

void MainWindow::on_pushButton_Random_clicked()
{
    pController->randomNextTrack();
}






MainWindow::~MainWindow()
{
    for (size_t i = 0; i < tracks.size(); i++)
    {
        delete tracks[i];
    }
    tracks.clear();

    delete pController;
    delete ui;
}
