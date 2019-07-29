#pragma once

// Qt
#include <QMainWindow>

// STL
#include <string>
#include <vector>


class Controller;
class TrackWidget;
class WaitWindow;

namespace Ui
{
    class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:

    void signalShowMessageBox(bool errorBox, std::string text);
    void signalSetTrack(size_t iTrackIndex);
    void signalSetNumber(size_t iNumber);
    void signalRemoveTrack(size_t iTrackIndex);
    void signalAddNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime);

    // WaitWindow
    void signalShowWaitWindow();
    void signalHideWaitWindow();
    void signalSetProgress(int value);

public:

    explicit MainWindow(QWidget *parent = nullptr);



    void showMessageBox(bool errorBox, std::string text);

    void addNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime);
    void removePlayingOnTrack(size_t iTrackIndex);
    void setPlayingOnTrack(size_t iTrackIndex);

    // Focus
    void setFocusOnTrack(size_t index);

    // WaitWindow
    void showWaitWindow();
    void hideWaitWindow();
    void setProgress(int value);

    void removeTrack(size_t iTrackIndex);

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

protected:

    void keyPressEvent(QKeyEvent* ev);

private slots:

    void slotShowMessageBox(bool errorBox, std::string text);
    void slotSetNumber(size_t iNumber);
    void slotAddNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime);
    void slotSetTrack(size_t iTrackIndex);

    // WaitWindow
    void slotShowWaitWindow();
    void slotHideWaitWindow();
    void slotSetProgress(int value);

    // Buttons
    void on_pushButton_Play_clicked();
    void on_pushButton_pause_clicked();
    void on_pushButton_stop_clicked();
    void on_pushButton_next_clicked();
    void on_pushButton_prev_clicked();

    // Volume slider
    void on_horizontalSlider_valueChanged(int value);

    // Menu
    void on_actionOpen_triggered();
    void on_actionOpen_Directory_triggered();
    void on_actionAbout_triggered();

private:

    Ui::MainWindow *ui;
    Controller* pController;
    WaitWindow* pWaitWindow;

    std::vector<TrackWidget*> tracks;

    int iSelectedTrackIndex;

    bool bSystemReady;
};
