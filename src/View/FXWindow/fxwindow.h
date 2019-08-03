#pragma once

#include <QMainWindow>

namespace Ui
{
    class FXWindow;
}

class QCloseEvent;


class FXWindow : public QMainWindow
{
    Q_OBJECT

signals:

    void signalChangePan          (float fPan);
    void signalChangePitch        (float fPitch);
    void signalChangeSpeedByPitch (float fSpeed);
    void signalChangeSpeedByTime  (float fSpeed);
    void signalChangeReverbVolume (float fVolume);
    void signalChangeEchoVolume   (float fEchoVolume);
    void signalOpenVST            (wchar_t* path);
    void signalShowVST            ();

public:

    explicit FXWindow(QWidget *parent = nullptr);

    ~FXWindow();

public slots:

    void slotSetVSTName(QString name);
    void slotResetAll();

protected:

    void closeEvent(QCloseEvent* ev);

private slots:

    // Pan
    void on_pushButton_pan_clicked();
    void on_horizontalSlider_pan_valueChanged(int value);

    // Pitch
    void on_pushButton_pitch_clicked();
    void on_horizontalSlider_pitch_valueChanged(int value);

    // Speed (by pitch)
    void on_pushButton_speed_by_pitch_clicked();
    void on_horizontalSlider_speed_by_pitch_valueChanged(int value);

    // Speed (by time)
    void on_pushButton_speed_by_time_clicked();
    void on_horizontalSlider_speed_by_time_valueChanged(int value);

    // Reverb
    void on_pushButton_reverb_clicked();
    void on_horizontalSlider_reverb_valueChanged(int value);

    // Delay
    void on_pushButton_delay_clicked();
    void on_horizontalSlider_delay_valueChanged(int value);

    // VST
    void on_pushButton_vst_clicked();

    // Set all to default
    void on_pushButton_clicked();

private:

    Ui::FXWindow *ui;

    bool bVSTLoaded;
};
