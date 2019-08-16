#pragma once

// Qt
#include <QMainWindow>

// STL
#include <string>
#include <vector>


class Controller;
class TrackWidget;
class WaitWindow;
class FXWindow;
class VSTWindow;

class QHideEvent;
class QSystemTrayIcon;
class QMouseEvent;
class QCPItemText;
class QCPItemRect;

namespace Ui
{
    class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:

    void signalShowMessageBox(bool errorBox, std::string text);
    void signalSetTrack(size_t iTrackIndex, bool bClear = false);
    void signalSetNumber(size_t iNumber);
    void signalAddNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime);

    // WaitWindow
    void signalShowWaitWindow(QString text);
    void signalHideWaitWindow();
    void signalSetProgress(int value);

    // Graph
    void signalClearGraph(bool stopTrack = false);
    void signalSetXMaxToGraph(unsigned int iMaxX);
    void signalAddDataToGraph(short int* pData, unsigned int iSizeInSamples, unsigned int iSamplesInOne);
    void signalSetCurrentPos(double x, std::string time);

    // VST
    void signalSetVSTName(QString name);
    void signalResetAll();
    void signalHideVSTWindow();

public:

    explicit MainWindow(QWidget *parent = nullptr);



    void showMessageBox(bool errorBox, std::string text);

    void addNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime);
    void removePlayingOnTrack(size_t iTrackIndex);
    void setPlayingOnTrack(size_t iTrackIndex, bool bClear = false);
    void uncheckRandomTrackButton();
    void uncheckRepeatTrackButton();

    void clearCurrentPlaylist();

    // VST
    HWND getVSTWindowHWND();
    void setVSTName(std::string name);
    void hideVSTWindow();

    // Graph
    void clearGraph(bool stopTrack = false);
    void setXMaxToGraph(unsigned int iMaxX);
    void addDataToGraph(short int* pData, unsigned int iSizeInSamples, unsigned int iSamplesInOne);
    void setCurrentPos(double x, std::string time);

    // Focus
    void setFocusOnTrack(size_t index);

    // WaitWindow
    void showWaitWindow(std::string text);
    void hideWaitWindow();
    void setProgress(int value);

    void markAnError();
    bool isSystemReady();



    ~MainWindow();

public slots:

    // Context menu on TrackWidget
    // This is also called from keyPressEvent().
    void slotMoveUp();
    void slotMoveDown();
    void deleteSelectedTrack();

    void slotTrackSelected(size_t iTrackIndex);
    void slotClickedOnTrack(size_t trackIndex);
    void slotDrop(QStringList paths);

    // Tray icon
    void slotShowWindow();

protected:

    void keyPressEvent(QKeyEvent* ev);
    void hideEvent(QHideEvent* ev);

private slots:

    void slotShowMessageBox(bool errorBox, std::string text);
    void slotSetNumber(size_t iNumber);
    void slotAddNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime);
    void slotSetTrack(size_t iTrackIndex, bool bClear = false);

    // VST
    void slotHideVSTWindow();

    // WaitWindow
    void slotShowWaitWindow(QString text);
    void slotHideWaitWindow();
    void slotSetProgress(int value);

    // Graph
    void slotClearGraph(bool stopTrack = false);
    void slotSetXMaxToGraph(unsigned int iMaxX);
    void slotAddDataToGraph(short int* pData, unsigned int iSizeInSamples, unsigned int iSamplesInOne);
    void slotSetCurrentPos(double x, std::string time);
    void slotClickOnGraph(QMouseEvent* ev);

    // FX
    void slotSetPan          (float fPan);
    void slotSetPitch        (float fPitch);
    void slotSetSpeedByPitch (float fSpeed);
    void slotSetSpeedByTime  (float fSpeed);
    void slotSetReverbVolume (float fVolume);
    void slotSetEchoVolume   (float fEchoVolume);
    void slotLoadVST         (wchar_t* pPath);
    void slotShowVST         ();
    void slotUnloadVST       ();
    void slotUpdate          ();

    // Buttons
    void on_pushButton_Play_clicked();
    void on_pushButton_pause_clicked();
    void on_pushButton_stop_clicked();
    void on_pushButton_next_clicked();
    void on_pushButton_prev_clicked();

    // Buttons under the volume slider
    void on_pushButton_repeat_clicked();
    void on_pushButton_Random_clicked();
    void on_pushButton_fx_clicked();
    void on_pushButton_clearPlaylist_clicked();

    // Volume slider
    void on_horizontalSlider_valueChanged(int value);

    // Menu
    void on_actionOpen_triggered();
    void on_actionOpen_Directory_triggered();
    void on_actionAbout_triggered();

    // Tracklist
    void on_actionSave_triggered();

    void on_actionOpen_2_triggered();

private:

    Ui::MainWindow*  ui;
    Controller*      pController;
    WaitWindow*      pWaitWindow;
    FXWindow*        pFXWindow;
    VSTWindow*       pVSTWindow;

    QSystemTrayIcon* pTrayIcon;
    QCPItemText*     pGraphTextTrackTime;
    QCPItemRect*     backgnd;

    std::vector<TrackWidget*> tracks;

    int iSelectedTrackIndex;

    unsigned int iCurrentXPosOnGraph;
    double minPosOnGraphForText;
    double maxPosOnGraphForText;

    bool bSystemReady;
};
