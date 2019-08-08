#include "mainwindow.h"
#include "ui_mainwindow.h"

// Qt
#include <QMessageBox>
#include <QFileDialog>
#include <QKeyEvent>
#include <QScrollBar>
#include <QSystemTrayIcon>
#include <QHideEvent>
#include <QVector>
#include <QMouseEvent>

// STL
#include <cmath>

// Custom
#include "../src/Controller/controller.h"
#include "../src/View/TrackWidget/trackwidget.h"
#include "../src/View/WaitWindow/waitwindow.h"
#include "../src/View/FXWindow/fxwindow.h"
#include "../src/View/VSTWindow/vstwindow.h"
#include "../src/globalparams.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->verticalLayout_Tracks->setAlignment( Qt::AlignTop );
    ui->horizontalSlider->setValue(static_cast<int>(DEFAULT_VOLUME*100));
    ui->label_volume->setText("Volume: " + QString::number(DEFAULT_VOLUME*100) + "%");

    // Tray icon
    pTrayIcon = new QSystemTrayIcon(this);
    pTrayIcon->setIcon( QIcon(":/bloodyLogo2.png") );
    connect(pTrayIcon, &QSystemTrayIcon::activated, this, &MainWindow::slotShowWindow);

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
    connect(this, &MainWindow::signalSetTrack,       this, &MainWindow::slotSetTrack);
    connect(this, &MainWindow::signalAddNewTrack,    this, &MainWindow::slotAddNewTrack);
    connect(this, &MainWindow::signalClearGraph,     this, &MainWindow::slotClearGraph);
    connect(this, &MainWindow::signalSetXMaxToGraph, this, &MainWindow::slotSetXMaxToGraph);
    connect(this, &MainWindow::signalAddDataToGraph, this, &MainWindow::slotAddDataToGraph);
    connect(this, &MainWindow::signalSetCurrentPos,  this, &MainWindow::slotSetCurrentPos);
    connect(this, &MainWindow::signalHideVSTWindow,  this, &MainWindow::slotHideVSTWindow);

    // Tracklist connects
    connect(ui->scrollArea, &TrackList::signalDrop, this, &MainWindow::slotDrop);

    // Graph
    ui->widget_graph->addGraph();
    ui->widget_graph->xAxis->setRange(0, MAX_X_AXIS_VALUE);
    ui->widget_graph->yAxis->setRange(0, MAX_AMPLITUDE);
    QPen pen;
    pen.setWidth(3);
    pen.setColor(QColor(Qt::darkRed));
    ui->widget_graph->graph(0)->setPen(pen);
    ui->widget_graph->setBackground(QColor(24, 24, 24));
    ui->widget_graph->xAxis->grid()->setVisible(false);
    ui->widget_graph->yAxis->grid()->setVisible(false);
    ui->widget_graph->xAxis->setTicks(false);
    ui->widget_graph->yAxis->setTicks(false);
    ui->widget_graph->graph(0)->setLineStyle(QCPGraph::LineStyle::lsImpulse);
    ui->widget_graph->axisRect()->setAutoMargins(QCP::msNone);
    ui->widget_graph->axisRect()->setMargins(QMargins(0,0,0,0));
    connect(ui->widget_graph, &QCustomPlot::mousePress, this, &MainWindow::slotClickOnGraph);
    pGraphTextTrackTime = new QCPItemText(ui->widget_graph);
    pGraphTextTrackTime->position->setType(QCPItemPosition::ptAxisRectRatio);
    pGraphTextTrackTime->position->setCoords(0, 0.5);
    pGraphTextTrackTime->setFont( QFont("Segoe UI", 10) );
    pGraphTextTrackTime->setColor(Qt::white);
    pGraphTextTrackTime->setPen(Qt::NoPen);
    pGraphTextTrackTime->setSelectedPen(Qt::NoPen);
    pGraphTextTrackTime->setText("");

    iCurrentXPosOnGraph = 0;

    // 3%
    minPosOnGraphForText = MAX_X_AXIS_VALUE * 3 / 100;
    minPosOnGraphForText /= static_cast<double>(MAX_X_AXIS_VALUE);
    maxPosOnGraphForText = MAX_X_AXIS_VALUE * 97 / 100;
    maxPosOnGraphForText /= static_cast<double>(MAX_X_AXIS_VALUE);

    // FXWindow
    pFXWindow = new FXWindow(this);
    pFXWindow->setWindowModality(Qt::WindowModality::WindowModal);
    connect(pFXWindow, &FXWindow::signalChangePan,          this, &MainWindow::slotSetPan);
    connect(pFXWindow, &FXWindow::signalChangePitch,        this, &MainWindow::slotSetPitch);
    connect(pFXWindow, &FXWindow::signalChangeSpeedByPitch, this, &MainWindow::slotSetSpeedByPitch);
    connect(pFXWindow, &FXWindow::signalChangeSpeedByTime,  this, &MainWindow::slotSetSpeedByTime);
    connect(pFXWindow, &FXWindow::signalChangeReverbVolume, this, &MainWindow::slotSetReverbVolume);
    connect(pFXWindow, &FXWindow::signalChangeEchoVolume,   this, &MainWindow::slotSetEchoVolume);
    connect(pFXWindow, &FXWindow::signalOpenVST,            this, &MainWindow::slotLoadVST);
    connect(pFXWindow, &FXWindow::signalShowVST,            this, &MainWindow::slotShowVST);
    connect(this, &MainWindow::signalResetAll,              pFXWindow, &FXWindow::slotResetAll);
    connect(this, &MainWindow::signalSetVSTName,            pFXWindow, &FXWindow::slotSetVSTName);

    // VSTWindow
    pVSTWindow = new VSTWindow(this);
    pVSTWindow->setWindowModality(Qt::WindowModality::WindowModal);
    connect(pVSTWindow, &VSTWindow::unloadVST, this, &MainWindow::slotUnloadVST);
    connect(pVSTWindow, &VSTWindow::updateAudio, this, &MainWindow::slotUpdate);

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

HWND MainWindow::getVSTWindowHWND()
{
    return pVSTWindow->getVSTWidgetHandle();
}

void MainWindow::setVSTName(std::string name)
{
    emit signalSetVSTName(QString::fromStdString(name));

    slotShowVST();
}

void MainWindow::hideVSTWindow()
{
    emit signalHideVSTWindow();
}

void MainWindow::clearGraph()
{
    emit signalClearGraph();
}

void MainWindow::setXMaxToGraph(unsigned int iMaxX)
{
    emit signalSetXMaxToGraph(iMaxX);
}

void MainWindow::addDataToGraph(char *pData, unsigned int iSizeInBytes)
{
    emit signalAddDataToGraph(pData, iSizeInBytes);
}

void MainWindow::setCurrentPos(int x, std::string time)
{
    emit signalSetCurrentPos(x, time);
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
    QMessageBox::information(nullptr, "Bloody Player", "Bloody Player v1.11");
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

void MainWindow::slotShowWindow()
{
    pTrayIcon->hide();
    raise();
    activateWindow();
    showNormal();
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

void MainWindow::hideEvent(QHideEvent *ev)
{
    hide();
    pTrayIcon->show();
    ev->ignore();
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

void MainWindow::slotClearGraph()
{
    ui->widget_graph->graph(0)->data()->clear();

    pGraphTextTrackTime->setText("");

    ui->widget_graph->replot();

    iCurrentXPosOnGraph = 0;
}

void MainWindow::slotSetXMaxToGraph(unsigned int iMaxX)
{
    ui->widget_graph->xAxis->setRange(0, iMaxX);
}

void MainWindow::slotAddDataToGraph(char *pData, unsigned int iSizeInBytes)
{
    QVector<double> x;
    QVector<double> y;

    for (unsigned int i = 0; i < iSizeInBytes;)
    {
        if (i + 4 >= iSizeInBytes) break;

        x.push_back( static_cast<double>(iCurrentXPosOnGraph) );
        iCurrentXPosOnGraph++;

        short int iSampleLeft;
        // Pointers magic
        // High byte
        std::memcpy(reinterpret_cast<char*>(&iSampleLeft) + 1, pData + i, sizeof(char));
        i++;
        // Low byte
        std::memcpy(reinterpret_cast<char*>(&iSampleLeft), pData + i, sizeof(char));
        i++;

        short int iSampleRight;
        // Pointers magic
        // High byte
        std::memcpy(reinterpret_cast<char*>(&iSampleRight) + 1, pData + i, sizeof(char));
        i++;
        // Low byte
        std::memcpy(reinterpret_cast<char*>(&iSampleRight), pData + i, sizeof(char));
        i++;

        int iSample = (iSampleLeft + iSampleRight) / 2;

        y.push_back( static_cast<double>(iSample) );
    }

    ui->widget_graph->graph(0)->addData(x, y, true);

    ui->widget_graph->replot();

    delete[] pData;
}

void MainWindow::slotSetCurrentPos(int x, std::string time)
{
    QVector<double> xVec;
    QVector<double> yVec;

    for (iCurrentXPosOnGraph; iCurrentXPosOnGraph < x; iCurrentXPosOnGraph++)
    {
        xVec.push_back(iCurrentXPosOnGraph);
        yVec.push_back(MAX_AMPLITUDE);
    }

    ui->widget_graph->graph(0)->addData(xVec, yVec, true);

    pGraphTextTrackTime->setText(QString::fromStdString(time));

    double currentPos = iCurrentXPosOnGraph / static_cast<double>(MAX_X_AXIS_VALUE);

    if ( (currentPos > (minPosOnGraphForText + 0.03)) && (currentPos < maxPosOnGraphForText) )
    {
        pGraphTextTrackTime->position->setCoords(currentPos - 0.03, 0.5);
    }
    else
    {
        if (currentPos <= minPosOnGraphForText + 0.03)      pGraphTextTrackTime->position->setCoords(minPosOnGraphForText, 0.5);
        else if (currentPos >= maxPosOnGraphForText) pGraphTextTrackTime->position->setCoords(maxPosOnGraphForText - 0.03, 0.5);
    }

    ui->widget_graph->replot();
}

void MainWindow::slotClickOnGraph(QMouseEvent* ev)
{
    pController->setTrackPos( static_cast<unsigned int>(ui->widget_graph->xAxis->pixelToCoord(ev->pos().x())) );
}

void MainWindow::slotSetPan(float fPan)
{
    pController->setPan(fPan);
}

void MainWindow::slotSetPitch(float fPitch)
{
    pController->setPitch(fPitch);
}

void MainWindow::slotSetSpeedByPitch(float fSpeed)
{
    pController->setSpeedByPitch(fSpeed);
}

void MainWindow::slotSetSpeedByTime(float fSpeed)
{
    pController->setSpeedByTime(fSpeed);
}

void MainWindow::slotSetReverbVolume(float fVolume)
{
    pController->setReverbVolume(fVolume);
}

void MainWindow::slotSetEchoVolume(float fEchoVolume)
{
    pController->setEchoVolume(fEchoVolume);
}

void MainWindow::slotLoadVST(wchar_t *pPath)
{
    pController->loadVSTPlugin(pPath);
}

void MainWindow::slotShowVST()
{
    pVSTWindow->show();
}

void MainWindow::slotUnloadVST()
{
    pVSTWindow->hide();
    pController->unloadVSTPlugin();

    emit signalSetVSTName("NULL");
}

void MainWindow::slotUpdate()
{
    pController->systemUpdate();
    // We will reset all effects because some changes to vst does not apply until you reset or change current effects
    emit signalResetAll();
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

void MainWindow::slotHideVSTWindow()
{
    if (pVSTWindow)
    {
        pVSTWindow->hide();
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

        slotClearGraph();

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

void MainWindow::on_pushButton_fx_clicked()
{
    pFXWindow->show();
}






MainWindow::~MainWindow()
{
    if (pVSTWindow) delete pVSTWindow;
    delete pFXWindow;

    delete pTrayIcon;

    for (size_t i = 0; i < tracks.size(); i++)
    {
        delete tracks[i];
    }
    tracks.clear();

    delete pController;
    delete ui;
}
