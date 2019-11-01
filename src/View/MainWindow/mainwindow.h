#pragma once




// Qt
#include <QMainWindow>

// STL
#include <string>
#include <mutex>
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


    // WaitWindow

        void     signalShowWaitWindow      (QString text);
        void     signalSetProgress         (int     value);
        void     signalHideWaitWindow      ();


    // Oscillogram

        void     signalAddDataToGraph      (float* pData,  unsigned int iSizeInSamples,  unsigned int iSamplesInOne);
        void     signalSetCurrentPos       (double x,          std::string time);
        void     signalClearGraph          (bool stopTrack = false);
        void     signalSetXMaxToGraph      (unsigned int iMaxX);


    // VST

        void     signalSetVSTName          (QString name);
        void     signalResetAll            ();
        void     signalHideVSTWindow       ();


    // Other

        void     signalAddNewTrack         (std::wstring trackName,  std::wstring trackInfo,  std::string trackTime);
        void     signalShowAllTracks       ();
        void     signalSetTrack            (size_t iTrackIndex,      bool bClear = false);
        void     signalShowMessageBox      (bool errorBox,           std::string text);
        void     signalSetNumber           (size_t iNumber);




public:

    explicit MainWindow                (QWidget *parent = nullptr);





    // Start functions

        void markAnError();
        bool isSystemReady();


    // VST

        void     setVSTName                (std::string name);
        HWND     getVSTWindowHWND          ();
        void     hideVSTWindow             ();


    // Oscillogram

        void     addDataToGraph            (float* pData,  unsigned int iSizeInSamples,  unsigned int iSamplesInOne);
        void     setCurrentPos             (double x,          std::string time);
        void     clearGraph                (bool stopTrack = false);
        void     setXMaxToGraph            (unsigned int iMaxX);


    // Focus

        void     setFocusOnTrack           (size_t index);


    // WaitWindow

        void     showWaitWindow            (std::string text);
        void     setProgress               (int value);
        void     hideWaitWindow            ();


    // Other

        void     addNewTrack               (std::wstring trackName,  std::wstring trackInfo,  std::string trackTime);
        void     showAllTracks             ();
        void     showMessageBox            (bool errorBox,           std::string text);
        void     setPlayingOnTrack         (size_t iTrackIndex,      bool bClear = false);
        void     removePlayingOnTrack      (size_t iTrackIndex);
        void     uncheckRandomTrackButton  ();
        void     uncheckRepeatTrackButton  ();
        void     clearCurrentPlaylist      ();




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

    // VST

        void  slotHideVSTWindow                    ();


    // WaitWindow

        void  slotShowWaitWindow                   (QString text);
        void  slotSetProgress                      (int value);
        void  slotHideWaitWindow                   ();


    // Oscillogram

        void  slotAddDataToGraph                   (float* pData,  unsigned int iSizeInSamples,  unsigned int iSamplesInOne);
        void  slotSetCurrentPos                    (double x,          std::string time);
        void  slotClearGraph                       (bool stopTrack = false);
        void  slotSetXMaxToGraph                   (unsigned int iMaxX);
        void  slotClickOnGraph                     (QMouseEvent* ev);


    // FX

        void  slotSetPan                           (float     fPan);
        void  slotSetPitch                         (float     fPitch);
        void  slotSetSpeedByPitch                  (float     fSpeed);
        void  slotSetSpeedByTime                   (float     fSpeed);
        void  slotSetReverbVolume                  (float     fVolume);
        void  slotSetEchoVolume                    (float     fEchoVolume);
        void  slotLoadVST                          (wchar_t*  pPath);
        void  slotShowVST                          ();
        void  slotUnloadVST                        ();
        void  slotUpdate                           ();


    // Main buttons

        void  on_pushButton_Play_clicked           ();
        void  on_pushButton_pause_clicked          ();
        void  on_pushButton_stop_clicked           ();
        void  on_pushButton_next_clicked           ();
        void  on_pushButton_prev_clicked           ();


    // Buttons under the volume slider

        void  on_pushButton_repeat_clicked         ();
        void  on_pushButton_Random_clicked         ();
        void  on_pushButton_fx_clicked             ();
        void  on_pushButton_clearPlaylist_clicked  ();


    // Volume slider

        void  on_horizontalSlider_valueChanged     (int value);


    // Menu

        void  on_actionOpen_triggered              ();
        void  on_actionOpen_Directory_triggered    ();
        void  on_actionAbout_triggered             ();


    // Tracklist

        void  on_actionSave_triggered              ();
        void  on_actionOpen_2_triggered            ();


    // Other

        void  slotAddNewTrack                      (std::wstring trackName,  std::wstring trackInfo,  std::string trackTime);
        void  slotShowAllTracks                    ();
        void  slotSetTrack                         (size_t iTrackIndex,      bool bClear = false);
        void  slotShowMessageBox                   (bool errorBox,           std::string text);
        void  slotSetNumber                        (size_t iNumber);


private:

    Ui::MainWindow*  ui;
    Controller*      pController;
    WaitWindow*      pWaitWindow;
    FXWindow*        pFXWindow;
    VSTWindow*       pVSTWindow;


    QSystemTrayIcon* pTrayIcon;
    QCPItemText*     pGraphTextTrackTime;
    QCPItemRect*     backgnd;


    std::mutex       mtxAddTrackWidget;


    int iSelectedTrackIndex;


    double minPosOnGraphForText;
    double maxPosOnGraphForText;


    unsigned int iCurrentXPosOnGraph;


    bool bSystemReady;


    std::vector<TrackWidget*> tracks;
};
