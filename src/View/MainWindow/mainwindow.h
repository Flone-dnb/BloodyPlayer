#pragma once
#include <QMainWindow>

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

    void signalSetTrack(size_t iTrackIndex);
    void signalSetNumber(size_t iNumber);
    void signalRemoveTrack(size_t iTrackIndex);

    void signalAddNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime);

    void signalShowWaitWindow();
    void signalHideWaitWindow();
    void signalSetProgressMax(int value);
    void signalSetProgress(int value);

public:

    explicit MainWindow(QWidget *parent = nullptr);

    void showMessageBox(bool errorBox, std::string text);

    void addNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime);

    void removePlayingOnTrack(size_t iTrackIndex);
    void setPlayingOnTrack(size_t iTrackIndex);
    void setFocusOnLastTrack();

    void showWaitWindow();
    void hideWaitWindow();
    void setProgressMax(int value);
    void setProgress(int value);

    void removeTrack(size_t iTrackIndex);

    ~MainWindow();

public slots:

    void slotDeleteSelectedTrack();

    void slotTrackSelected(size_t iTrackIndex);

    void slotClickedOnTrack(size_t trackIndex);

    void slotDrop(QStringList paths);

private slots:

    void slotSetNumber(size_t iNumber);

    void slotRemoveTrack(size_t iTrackIndex);

    void slotAddNewTrack(std::wstring trackName, std::wstring trackInfo, std::string trackTime);

    void slotShowWaitWindow();
    void slotHideWaitWindow();
    void slotSetProgressMax(int value);
    void slotSetProgress(int value);

    void slotSetTrack(size_t iTrackIndex);

    void on_actionOpen_triggered();

    void on_pushButton_Play_clicked();

    void on_actionAbout_triggered();

    void on_pushButton_pause_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_next_clicked();

    void on_pushButton_prev_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_actionOpen_Directory_triggered();

private:

    Ui::MainWindow *ui;

    std::vector<TrackWidget*> tracks;

    Controller* pController;
    WaitWindow* pWaitWindow;

    int iSelectedTrack;
};
