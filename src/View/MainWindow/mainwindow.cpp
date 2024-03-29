﻿// This file is part of the Bloody Player.
// Copyright Aleksandr "Flone" Tretyakov (github.com/Flone-dnb).
// Licensed under the ZLib license.
// Refer to the LICENSE file included.

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
#include <thread>

// Custom
#include "Controller/controller.h"
#include "View/TrackWidget/trackwidget.h"
#include "View/WaitWindow/waitwindow.h"
#include "View/FXWindow/fxwindow.h"
#include "View/VSTWindow/vstwindow.h"
#include "View/AboutWindow/aboutwindow.h"
#include "View/SearchWindow/searchwindow.h"
#include "globalparams.h"

#if _WIN32
using std::memcpy;
#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    bSystemReady = false;

    ui->setupUi(this);

    pController = new Controller(this);

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
    connect(this, &MainWindow::signalShowWaitWindow,      this, &MainWindow::slotShowWaitWindow);
    connect(this, &MainWindow::signalHideWaitWindow,      this, &MainWindow::slotHideWaitWindow);
    connect(this, &MainWindow::signalSetProgress,         this, &MainWindow::slotSetProgress);
    connect(this, &MainWindow::signalSetNumber,           this, &MainWindow::slotSetNumber);
    connect(this, &MainWindow::signalShowMessageBox,      this, &MainWindow::slotShowMessageBox);
    connect(this, &MainWindow::signalSetTrack,            this, &MainWindow::slotSetTrack);
    connect(this, &MainWindow::signalAddNewTrack,         this, &MainWindow::slotAddNewTrack);
    connect(this, &MainWindow::signalClearGraph,          this, &MainWindow::slotClearGraph);
    connect(this, &MainWindow::signalSetXMaxToGraph,      this, &MainWindow::slotSetXMaxToGraph);
    connect(this, &MainWindow::signalAddDataToGraph,      this, &MainWindow::slotAddDataToGraph);
    connect(this, &MainWindow::signalSetCurrentPos,       this, &MainWindow::slotSetCurrentPos);
#if _WIN32
    connect(this, &MainWindow::signalHideVSTWindow,       this, &MainWindow::slotHideVSTWindow);
#endif
    connect(this, &MainWindow::signalSetRepeatPoint,      this, &MainWindow::slotSetRepeatPoint);
    connect(this, &MainWindow::signalEraseRepeatSection,  this, &MainWindow::slotEraseRepeatSection);
    connect(this, &MainWindow::signalSetTrackBitrate,     this, &MainWindow::slotSetTrackBitrate);
    connect(this, &MainWindow::signalClearPlaylist,       this, &MainWindow::slotClearPlaylist);

    // Tracklist connect
    connect(ui->scrollArea, &TrackList::signalDrop, this, &MainWindow::slotDrop);



    // Graph
    ui->widget_graph->addGraph();
    ui->widget_graph->xAxis->setRange(0, MAX_X_AXIS_VALUE);
    ui->widget_graph->yAxis->setRange(0.0, MAX_Y_AXIS_VALUE);

    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(Qt::darkRed));
    ui->widget_graph->graph(0)->setPen(pen);

    // color and stuff
    ui->widget_graph->setBackground(QColor(24, 24, 24));
    ui->widget_graph->xAxis->grid()->setVisible(false);
    ui->widget_graph->yAxis->grid()->setVisible(false);
    ui->widget_graph->xAxis->setTicks(false);
    ui->widget_graph->yAxis->setTicks(false);
    ui->widget_graph->graph(0)->setLineStyle(QCPGraph::LineStyle::lsLine);
    ui->widget_graph->axisRect()->setAutoMargins(QCP::msNone);
    ui->widget_graph->axisRect()->setMargins(QMargins(0,0,0,0));

    connect(ui->widget_graph, &QCustomPlot::mousePress, this, &MainWindow::slotClickOnGraph);


    // fill rect
    backgnd = new QCPItemRect(ui->widget_graph);
    backgnd->topLeft->setType(QCPItemPosition::ptAxisRectRatio);
    backgnd->topLeft->setCoords(0, 0);
    backgnd->bottomRight->setType(QCPItemPosition::ptAxisRectRatio);
    backgnd->bottomRight->setCoords(0, MAX_Y_AXIS_VALUE);
    backgnd->setBrush(QBrush(QColor(0, 0, 0, PLAYED_SECTION_ALPHA)));
    backgnd->setPen(Qt::NoPen);


    backgndRight = new QCPItemRect(ui->widget_graph);
    backgndRight->topLeft->setType(QCPItemPosition::ptAxisRectRatio);
    backgndRight->topLeft->setCoords(0, 0);
    backgndRight->bottomRight->setType(QCPItemPosition::ptAxisRectRatio);
    backgndRight->bottomRight->setCoords(0, MAX_Y_AXIS_VALUE);
    backgndRight->setBrush(QBrush(QColor(0, 0, 0, PLAYED_SECTION_ALPHA)));
    backgndRight->setPen(Qt::NoPen);


    // repeat left
    repeatLeft = new QCPItemRect(ui->widget_graph);
    repeatLeft->topLeft->setType(QCPItemPosition::ptAxisRectRatio);
    repeatLeft->topLeft->setCoords(0, 0);
    repeatLeft->bottomRight->setType(QCPItemPosition::ptAxisRectRatio);
    repeatLeft->bottomRight->setCoords(0, MAX_Y_AXIS_VALUE);
    repeatLeft->setBrush(QBrush(QColor(0, 0, 0, REPEAT_GRAYED_ALPHA)));
    repeatLeft->setPen(Qt::NoPen);


    // repeat right
    repeatRight = new QCPItemRect(ui->widget_graph);
    repeatRight->topLeft->setType(QCPItemPosition::ptAxisRectRatio);
    repeatRight->topLeft->setCoords(0, 0);
    repeatRight->bottomRight->setType(QCPItemPosition::ptAxisRectRatio);
    repeatRight->bottomRight->setCoords(0, MAX_Y_AXIS_VALUE);

    repeatRight->setBrush(QBrush(QColor(0, 0, 0, REPEAT_GRAYED_ALPHA)));
    repeatRight->setPen(Qt::NoPen);


    // text
    pGraphTextTrackTime = new QCPItemText(ui->widget_graph);
    pGraphTextTrackTime->position->setType(QCPItemPosition::ptAxisRectRatio);
    pGraphTextTrackTime->position->setCoords(0, 0.5);
    pGraphTextTrackTime->setFont( QFont("Segoe UI", 10) );
    pGraphTextTrackTime->setColor(Qt::white);
    pGraphTextTrackTime->setPen(Qt::NoPen);
    pGraphTextTrackTime->setSelectedPen(Qt::NoPen);
    pGraphTextTrackTime->setText("");





    iCurrentXPosOnGraph = 0;

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
#if _WIN32
    connect(pFXWindow, &FXWindow::signalOpenVST,            this, &MainWindow::slotLoadVST);
    connect(pFXWindow, &FXWindow::signalShowVST,            this, &MainWindow::slotShowVST);
#endif
    connect(this, &MainWindow::signalShowAllTracks,         this, &MainWindow::slotShowAllTracks);
    connect(this, &MainWindow::signalResetAll,              pFXWindow, &FXWindow::slotResetAll);
#if _WIN32
    connect(this, &MainWindow::signalSetVSTName,            pFXWindow, &FXWindow::slotSetVSTName);
#endif

    pVSTWindow = nullptr;
#if _WIN32
    // VSTWindow
    pVSTWindow = new VSTWindow(this);
    pVSTWindow->setWindowModality(Qt::WindowModality::WindowModal);
    connect(pVSTWindow, &VSTWindow::unloadVST, this, &MainWindow::slotUnloadVST);
    connect(pVSTWindow, &VSTWindow::updateAudio, this, &MainWindow::slotUpdate);
#endif
}






void MainWindow::showMessageBox(bool errorBox, std::string text)
{
    emit signalShowMessageBox(errorBox, QString::fromStdString(text));
}

void MainWindow::showWMessageBox(bool errorBox, std::wstring text)
{
    emit signalShowMessageBox(errorBox, QString::fromStdWString(text));
}

void MainWindow::addNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime)
{
    emit signalAddNewTrack(trackName, trackInfo, trackTime);
}

void MainWindow::showAllTracks()
{
    emit signalShowAllTracks();
}

void MainWindow::removePlayingOnTrack(size_t iTrackIndex)
{
    tracks[iTrackIndex]->disablePlaying();
}

void MainWindow::setPlayingOnTrack(size_t iTrackIndex, bool bClear)
{
    if (bClear)
    {
        mtxAddTrackWidget .lock();
        mtxAddTrackWidget .unlock();

        emit signalSetTrack(iTrackIndex, true);
    }
    else
    {
        mtxAddTrackWidget .lock();

        tracks[iTrackIndex]->setPlaying();

        ui->scrollArea->ensureWidgetVisible(tracks[iTrackIndex]);

        mtxAddTrackWidget .unlock();

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

void MainWindow::clearCurrentPlaylist()
{
    std::promise<bool> promiseResult;
    std::future<bool> future = promiseResult.get_future();

    emit signalClearPlaylist(&promiseResult);

    future.get();
}

void MainWindow::setTrackBitrate(size_t iNumber, std::string sBitrate)
{
    emit signalSetTrackBitrate(iNumber, QString::fromStdString(sBitrate));
}

size_t MainWindow::getTracksCount()
{
    size_t iCount = 0;

    mtxAddTrackWidget .lock();

    iCount = tracks .size();

    mtxAddTrackWidget .unlock();

    return iCount;
}

size_t MainWindow::getTracksCountOnScreen()
{
    size_t iVisibleTracks = 0;

    mtxAddTrackWidget .lock();

    for (size_t i = 0; i < tracks .size(); i++)
    {
        if ( tracks[i] ->isVisible() )
        {
            iVisibleTracks++;
        }
    }

    mtxAddTrackWidget .unlock();

    return iVisibleTracks;
}

#if _WIN32
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
#endif

void MainWindow::clearGraph(bool stopTrack)
{
    emit signalClearGraph(stopTrack);
}

void MainWindow::setXMaxToGraph(unsigned int iMaxX)
{
    emit signalSetXMaxToGraph(iMaxX);
}

void MainWindow::addDataToGraph(float* pData, unsigned int iSizeInSamples, unsigned int iSamplesInOne)
{
    emit signalAddDataToGraph(pData, iSizeInSamples, iSamplesInOne);
}

void MainWindow::setCurrentPos(double x, std::string time)
{
    emit signalSetCurrentPos(x, time);
}

void MainWindow::setRepeatPoint(bool bFirstPoint, double x)
{
    emit signalSetRepeatPoint(bFirstPoint, x);
}

void MainWindow::eraseRepeatSection()
{
    emit signalEraseRepeatSection();
}

void MainWindow::setFocusOnTrack(size_t index)
{
    mtxAddTrackWidget .lock();

    ui->scrollArea->ensureWidgetVisible(tracks[index], 50, 50);

    mtxAddTrackWidget .unlock();
}

void MainWindow::showWaitWindow(std::string text)
{
    emit signalShowWaitWindow(QString::fromStdString(text));
}

void MainWindow::hideWaitWindow()
{
    emit signalHideWaitWindow();
}

void MainWindow::setSearchMatchCount(size_t iMatches)
{
    emit signalSearchMatchCount (iMatches);
}

void MainWindow::searchSetSelected(size_t iTrackIndex)
{
    tracks[iTrackIndex]->enableSelected();
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
        std::vector<std::wstring> localPaths;

        for (int i = 0; i < files.size(); i++)
        {
            localPaths.push_back(files[i].toStdWString());
        }

        pController->addTracks(localPaths);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    AboutWindow* pAboutWindow = new AboutWindow ( QString::fromStdString(pController->getBloodyVersion()), this );
    pAboutWindow ->setWindowModality (Qt::WindowModality::WindowModal);
    pAboutWindow ->show();
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
    std::vector<std::wstring> localPaths;

    for (int i = 0; i < paths.size(); i++)
    {
        localPaths.push_back(paths[i].toStdWString());
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

void MainWindow::slotSearchFindPrev()
{
    pController->searchFindPrev ();
}

void MainWindow::slotSearchFindNext()
{
    pController->searchFindNext ();
}

void MainWindow::slotSearchTextSet(QString keyword)
{
    pController->searchTextSet (keyword.toStdWString());
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
    else if (ev->modifiers() == Qt::KeyboardModifier::ControlModifier && ev->key() == Qt::Key_F)
    {
        SearchWindow* pSearchWindow = new SearchWindow (this);
        pSearchWindow->setWindowModality(Qt::WindowModal);

        connect (pSearchWindow, &SearchWindow::findPrev,             this,          &MainWindow::slotSearchFindPrev);
        connect (pSearchWindow, &SearchWindow::findNext,             this,          &MainWindow::slotSearchFindNext);
        connect (pSearchWindow, &SearchWindow::searchTextChanged,    this,          &MainWindow::slotSearchTextSet);
        connect (this,          &MainWindow::signalSearchMatchCount, pSearchWindow, &SearchWindow::searchMatchCount);

        pSearchWindow->show();
    }
}

void MainWindow::hideEvent(QHideEvent *ev)
{
    //hide();
    //pTrayIcon->show();
    //ev->ignore();
}

void MainWindow::slotShowMessageBox(bool errorBox, QString text)
{
    if (errorBox)
    {
        QMessageBox::warning(nullptr, "Error", text);
    }
    else
    {
        QMessageBox::information(nullptr, "Information", text);
    }
}

void MainWindow::slotSetNumber(size_t iNumber)
{
    tracks[iNumber]->setNumber(iNumber+1);
}

void MainWindow::slotSetTrackBitrate(size_t iNumber, QString sBitrate)
{
    tracks[iNumber]->setBitrate(sBitrate);
}

void MainWindow::slotUpdateTrackInfo(size_t iTrackIndex)
{
    mtxAddTrackWidget .lock();

    ui ->label_TrackInfo ->setText( tracks[iTrackIndex] ->trackInfo );

    mtxAddTrackWidget .unlock();
}

void MainWindow::slotClearPlaylist(std::promise<bool>* pPromiseResult)
{
    if (tracks.size() > 0)
    {
        mtxAddTrackWidget .lock();

        for (size_t i = 0; i < tracks.size(); i++)
        {
            delete tracks[i];
        }
        tracks.clear();

        ui->label_TrackName->setText( "Track Name" );
        ui->label_TrackInfo->setText( "Track Info" );

        slotClearGraph();

        //ui->horizontalSlider->setValue(static_cast<int>(DEFAULT_VOLUME*100));
        //ui->horizontalSlider->setEnabled(false);

        mtxAddTrackWidget .unlock();
    }

    pPromiseResult->set_value(false);
}

void MainWindow::slotAddNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime)
{
    mtxAddTrackWidget.lock();

    TrackWidget* pNewTrack = new TrackWidget( QString::fromStdWString(trackName), QString::fromStdWString(trackInfo),
                                              QString::fromStdString(trackTime) );

    connect(pNewTrack, &TrackWidget::signalDoubleClick,     this, &MainWindow::slotClickedOnTrack);
    connect(pNewTrack, &TrackWidget::signalSelected,        this, &MainWindow::slotTrackSelected);
    connect(pNewTrack, &TrackWidget::signalDelete,          this, &MainWindow::deleteSelectedTrack);
    connect(pNewTrack, &TrackWidget::signalMoveUp,          this, &MainWindow::slotMoveUp);
    connect(pNewTrack, &TrackWidget::signalMoveDown,        this, &MainWindow::slotMoveDown);
    connect(pNewTrack, &TrackWidget::signalUpdateTrackInfo, this, &MainWindow::slotUpdateTrackInfo);

    pNewTrack->setVisible(false);
    ui->verticalLayout_Tracks->addWidget(pNewTrack);

    tracks.push_back(pNewTrack);
    pNewTrack->setNumber( tracks.size() );
    //pNewTrack->show();

//    if (tracks.size() == 1)
//    {
//        ui->horizontalSlider->setEnabled(true);
//    }

    mtxAddTrackWidget.unlock();
}

void MainWindow::slotShowAllTracks()
{
    mtxAddTrackWidget .lock();

    for (size_t i = 0; i < tracks.size(); i++)
    {
        tracks[i]->setVisible(true);
    }

    mtxAddTrackWidget .unlock();

//    std::thread focus(&MainWindow::setFocusOnTrack, this, tracks.size() - 1);
//    focus.detach();
}

void MainWindow::slotShowWaitWindow(QString text)
{
    pWaitWindow = new WaitWindow(this);
    pWaitWindow->setWindowModality(Qt::WindowModality::ApplicationModal);
    pWaitWindow->setLabelText(text);
    //pWaitWindow->setLabelText("Please wait.\nAdding tracks.");
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

void MainWindow::slotClearGraph(bool stopTrack)
{
    pGraphTextTrackTime->setText("");
    backgnd->bottomRight->setCoords(0, 1);

    if (stopTrack == false)
    {
        iCurrentXPosOnGraph = 0;
        ui->widget_graph->graph(0)->data()->clear();
    }

    ui->widget_graph->replot();
}

void MainWindow::slotSetXMaxToGraph(unsigned int iMaxX)
{
    ui->widget_graph->xAxis->setRange(0, iMaxX);
}

void MainWindow::slotAddDataToGraph(float* pData, unsigned int iSizeInSamples, unsigned int iSamplesInOne)
{
    QVector<double> x;
    QVector<double> y;

    // Here we need to show only 'iSamplesInOne' samples out of all 'iSizeInSamples'.

    //                                            here '2' because 2 channels (TODO: get rid of this constant value),
    //                                            and '-1' because we read two values in one cycle.
    for (unsigned int i = 0; i < iSizeInSamples - (2 * iSamplesInOne - 1); i+= (2 * iSamplesInOne))
    {
        x.push_back( static_cast<double>(iCurrentXPosOnGraph) );
        iCurrentXPosOnGraph++;

        float fSamplePlus = -1.0f;
        float fSampleMinus = 1.0f;
        bool bPlusFirst = false;

        // Average all 'iSamplesInOne' samples in one sample.
        for (unsigned int j = 0; j < (2 * iSamplesInOne); j+= 2)
        {
            // L + R channels.
            float fSample1 = 0.0f;
            if (abs(pData[i + j]) < abs(pData[i + j + 1]))
            {
                fSample1 = pData[i + j];
            }
            else
            {
                fSample1 = pData[i + j + 1];
                //fSample1 = (pData[i + j] + pData[i + j + 1]) / 2;
            }

            if (j == 0)
            {
                if (fSample1 > 0.0f)
                {
                    bPlusFirst = true;
                }
                else
                {
                    bPlusFirst = false;
                }
            }

            if (fSample1 > 0.0f)
            {
                if (fSample1 > fSamplePlus)
                {
                    fSamplePlus = fSample1;
                }
            }
            else
            {
                if (fSample1 < fSampleMinus)
                {
                    fSampleMinus = fSample1;
                }
            }


//            if (fSample < 0.0f)
//            {
//                fSample = fSample1;
//            }
//            else
//            {
//                fSample = (fSample + fSample1) / 2;
//            }
        }

        fSamplePlus *= 0.5f;
        fSamplePlus = 0.5f + fSamplePlus;

        fSampleMinus *= 0.5f;
        fSampleMinus += 0.5f;

        if (bPlusFirst)
        {
            y.push_back( static_cast<double>(fSamplePlus) );

//            x.push_back( static_cast<double>(iCurrentXPosOnGraph) );
//            iCurrentXPosOnGraph++;

//            y.push_back( static_cast<double>(fSampleMinus) );
        }
        else
        {
            y.push_back( static_cast<double>(fSampleMinus) );

//            x.push_back( static_cast<double>(iCurrentXPosOnGraph) );
//            iCurrentXPosOnGraph++;

//            y.push_back( static_cast<double>(fSamplePlus) );
        }

    }

    ui->widget_graph->graph(0)->addData(x, y, true);

    ui->widget_graph->replot();

    delete[] pData;
}

void MainWindow::slotSetCurrentPos(double x, std::string time)
{
    backgnd->bottomRight->setCoords(x, 1);
    pGraphTextTrackTime->setText(QString::fromStdString(time));

    if ( (x > (minPosOnGraphForText + 0.03)) && (x < maxPosOnGraphForText) )
    {
        pGraphTextTrackTime->position->setCoords(x - 0.03, 0.5);
    }
    else
    {
        if (x <= minPosOnGraphForText + 0.03)      pGraphTextTrackTime->position->setCoords(minPosOnGraphForText, 0.5);
        else if (x >= maxPosOnGraphForText) pGraphTextTrackTime->position->setCoords(maxPosOnGraphForText - 0.03, 0.5);
    }

    ui->widget_graph->replot();
}

void MainWindow::slotSetRepeatPoint(bool bFirstPoint, double x)
{
    if (bFirstPoint)
    {
        repeatLeft->topLeft->setCoords(0, 0);
        repeatLeft->bottomRight->setCoords(x, MAX_Y_AXIS_VALUE);
    }
    else
    {
        backgndRight->topLeft->setCoords(x, 0);
        backgndRight->bottomRight->setCoords(1.0, MAX_Y_AXIS_VALUE);

        repeatRight->topLeft->setCoords(x, 0);
        repeatRight->bottomRight->setCoords(1.0, MAX_Y_AXIS_VALUE);
    }
}

void MainWindow::slotEraseRepeatSection()
{
    repeatLeft->topLeft->setCoords(0, 0);
    repeatLeft->bottomRight->setCoords(0, MAX_Y_AXIS_VALUE);

    repeatRight->topLeft->setCoords(0, 0);
    repeatRight->bottomRight->setCoords(0, MAX_Y_AXIS_VALUE);

    backgndRight->topLeft->setCoords(0, 0);
    backgndRight->bottomRight->setCoords(0, MAX_Y_AXIS_VALUE);
}

void MainWindow::slotClickOnGraph(QMouseEvent* ev)
{
    if (ev->button() == Qt::MouseButton::LeftButton)
    {
       pController->setTrackPos( static_cast<unsigned int>(ui->widget_graph->xAxis->pixelToCoord(ev->pos().x())) );
    }
    else if (ev->button() == Qt::MouseButton::RightButton)
    {
        pController->setRepeatPoint( static_cast<unsigned int>(ui->widget_graph->xAxis->pixelToCoord(ev->pos().x())) );
    }
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

#if _WIN32
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
#endif

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

#if _WIN32
void MainWindow::slotHideVSTWindow()
{
    if (pVSTWindow)
    {
        pVSTWindow->hide();
    }
}
#endif

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    pController->setVolume( value/100.0f );
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
            //ui->horizontalSlider->setValue(static_cast<int>(DEFAULT_VOLUME*100));
            //ui->horizontalSlider->setEnabled(false);

            ui->label_TrackName->setText( "Track Name" );
            ui->label_TrackInfo->setText( "Track Info" );

            slotClearGraph();
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
        mtxAddTrackWidget .lock();

        pController->clearPlaylist();

        for (size_t i = 0; i < tracks.size(); i++)
        {
            delete tracks[i];
        }
        tracks.clear();

        ui->label_TrackName->setText( "Track Name" );
        ui->label_TrackInfo->setText( "Track Info" );

        slotClearGraph();

        //ui->horizontalSlider->setValue(static_cast<int>(DEFAULT_VOLUME*100));
        //ui->horizontalSlider->setEnabled(false);

        mtxAddTrackWidget .unlock();
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

void MainWindow::on_actionSave_triggered()
{
    if (tracks.size() > 0)
    {
        QString file = QFileDialog::getSaveFileName(nullptr, "Save Tracklist", "", "Bloody Player Tracklist (*.bpt)");

        if (file != "")
        {
            pController->saveTracklist(file.toStdWString());
        }
    }
    else
    {
        QMessageBox::information(nullptr, "Tracklist", "The tracklist is empty. There is nothing to save.");
    }
}

void MainWindow::on_actionOpen_2_triggered()
{
    QString file = QFileDialog::getOpenFileName(nullptr, "Open Tracklist", "", "Bloody Player Tracklist (*.bpt)");
    if (file != "")
    {
        if (tracks.size() > 0)
        {
            QMessageBox::StandardButton answer = QMessageBox::question(nullptr, "Open Tracklist", "Do you want to clear current tracklist and add a new one? Select \"No\" if you want to add tracks from new tracklist to the current one.");
            if (answer == QMessageBox::StandardButton::Yes)
            {
                pController->openTracklist(file.toStdWString(), true);
            }
            else
            {
                pController->openTracklist(file.toStdWString(), false);
            }
        }
        else
        {
            pController->openTracklist(file.toStdWString(), false);
        }
    }
}





MainWindow::~MainWindow()
{
    mtxAddTrackWidget .lock();
    mtxAddTrackWidget .unlock();

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
