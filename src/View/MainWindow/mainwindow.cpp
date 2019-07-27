#include "mainwindow.h"
#include "ui_mainwindow.h"

// Qt
#include <QMessageBox>
#include <QFileDialog>

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

    iSelectedTrack = -1;

    // This to this
    connect(this, &MainWindow::signalShowWaitWindow, this, &MainWindow::slotShowWaitWindow);
    connect(this, &MainWindow::signalHideWaitWindow, this, &MainWindow::slotHideWaitWindow);
    connect(this, &MainWindow::signalSetProgress, this, &MainWindow::slotSetProgress);
    connect(this, &MainWindow::signalSetProgressMax, this, &MainWindow::slotSetProgressMax);
    connect(this, &MainWindow::signalRemoveTrack, this, &MainWindow::slotRemoveTrack);
    connect(this, &MainWindow::signalSetNumber, this, &MainWindow::slotSetNumber);

    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<std::wstring>("std::wstring");
    qRegisterMetaType<size_t>("size_t");

    // Tracklist connects
    connect(ui->scrollArea, &TrackList::signalDrop, this, &MainWindow::slotDrop);
    connect(ui->scrollArea, &TrackList::signalDeleteTrack, this, &MainWindow::slotDeleteSelectedTrack);

    connect(this, &MainWindow::signalSetTrack, this, &MainWindow::slotSetTrack);
    connect(this, &MainWindow::signalAddNewTrack, this, &MainWindow::slotAddNewTrack);

    pController = new Controller(this);
}




void MainWindow::showMessageBox(bool errorBox, std::string text)
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

void MainWindow::addNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime)
{
    emit signalAddNewTrack(trackName, trackInfo, trackTime);
}

void MainWindow::removePlayingOnTrack(size_t iTrackIndex)
{
    tracks[iTrackIndex]->disablePlaying();
}

void MainWindow::setPlayingOnTrack(size_t iTrackIndex)
{
    tracks[iTrackIndex]->setPlaing();

    ui->scrollArea->ensureWidgetVisible(tracks[iTrackIndex]);

    if (iSelectedTrack == iTrackIndex) iSelectedTrack = -1;

    emit signalSetTrack(iTrackIndex);
}

void MainWindow::setFocusOnLastTrack()
{
    ui->scrollArea->ensureWidgetVisible(tracks.back(), 50, 100);
}

void MainWindow::showWaitWindow()
{
    emit signalShowWaitWindow();
}

void MainWindow::hideWaitWindow()
{
    emit signalHideWaitWindow();
}

void MainWindow::setProgressMax(int value)
{
    emit signalSetProgressMax(value);
}

void MainWindow::setProgress(int value)
{
    emit signalSetProgress(value);
}

void MainWindow::removeTrack(size_t iTrackIndex)
{
    emit signalRemoveTrack(iTrackIndex);
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
    QMessageBox::information(nullptr, "Magic Player", "Magic Player v1.0");
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

void MainWindow::slotSetNumber(size_t iNumber)
{
    tracks[iNumber]->setNumber(iNumber+1);
}

void MainWindow::slotRemoveTrack(size_t iTrackIndex)
{
    iSelectedTrack = -1;

    delete tracks[iTrackIndex];
    tracks.erase(tracks.begin() + iTrackIndex);

    pController->removeTrack(iTrackIndex);

    for (size_t i = iTrackIndex; i < tracks.size(); i++)
    {
        emit signalSetNumber(i);
    }

    if (tracks.size() == 0)
    {
        ui->horizontalSlider->setValue(static_cast<int>(DEFAULT_VOLUME*100));
        ui->horizontalSlider->setEnabled(false);
    }
}

void MainWindow::slotAddNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime)
{
    TrackWidget* pNewTrack = new TrackWidget( QString::fromStdWString(trackName), QString::fromStdWString(trackInfo), QString::fromStdString(trackTime) );
    connect(pNewTrack, &TrackWidget::signalDoubleClick, this, &MainWindow::slotClickedOnTrack);
    connect(pNewTrack, &TrackWidget::signalSelected, this, &MainWindow::slotTrackSelected);
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

void MainWindow::slotSetProgressMax(int value)
{
    pWaitWindow->setProgressMax(value);
}

void MainWindow::slotSetProgress(int value)
{
    pWaitWindow->setProgressValue(value);
}

void MainWindow::slotSetTrack(size_t iTrackIndex)
{
    ui->label_TrackName->setText( tracks[iTrackIndex]->trackName );
    ui->label_TrackInfo->setText( tracks[iTrackIndex]->trackInfo );
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
    if (iSelectedTrack != static_cast<int>(iTrackIndex))
    {
        if (iSelectedTrack != -1)
        {
            tracks[iSelectedTrack]->disableSelected();
        }

        iSelectedTrack = static_cast<int>(iTrackIndex);
    }
    else
    {
        tracks[iSelectedTrack]->disableSelected();
        iSelectedTrack = -1;
    }
}

void MainWindow::slotDeleteSelectedTrack()
{
    if (iSelectedTrack != -1)
    {
        slotRemoveTrack(iSelectedTrack);
    }
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
