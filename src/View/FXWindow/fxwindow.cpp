#include "fxwindow.h"
#include "ui_fxwindow.h"

#include <QCloseEvent>
#include <QFileDialog>

FXWindow::FXWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FXWindow)
{
    ui->setupUi(this);

    // Hide maximize & minimize buttons
    Qt::WindowFlags flags = windowFlags();
    flags &= ~Qt::WindowMaximizeButtonHint;
    flags &= ~Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags);

    bVSTLoaded = false;
}





void FXWindow::on_pushButton_pan_clicked()
{
    ui->horizontalSlider_pan->setValue(0);
    ui->label_pan->setText("Pan: 0");

    emit signalChangePan(0.0f);
}

void FXWindow::on_horizontalSlider_pan_valueChanged(int value)
{
    float fPan = value / 100.0f;
    ui->label_pan->setText("Pan: " + QString::number(static_cast<double>(fPan), 'f', 2 ));

    emit signalChangePan(fPan);
}



void FXWindow::on_pushButton_pitch_clicked()
{
    ui->horizontalSlider_pitch->setValue(0);
    ui->label_pitch->setText("Pitch: 0");

    emit signalChangePitch(1.0f);
}

void FXWindow::on_horizontalSlider_pitch_valueChanged(int value)
{
    if (value > 0)
    {
        float fPitch = 1.0f;
        fPitch += value / 100.0f;

        ui->label_pitch->setText("Pitch: " + QString::number(static_cast<double>(fPitch) - 1.0, 'f', 2));
        emit signalChangePitch(fPitch);
    }
    else if (value < 0)
    {
        float fPitch = 1.0f;
        fPitch -= abs(value) / 100.0f / 2.0f;

        ui->label_pitch->setText("Pitch: " + QString::number(static_cast<double>(value) / 100.0, 'f', 2));
        emit signalChangePitch(fPitch);
    }
    else
    {
        ui->label_pitch->setText("Pitch: 0");
        emit signalChangePitch(1.0f);
    }
}

void FXWindow::on_pushButton_speed_by_pitch_clicked()
{
    if (ui->horizontalSlider_speed_by_pitch->value() != 100)
    {
        ui->horizontalSlider_speed_by_pitch->setValue(100);
        ui->label_speed_by_pitch->setText("Speed: x1");

        emit signalChangeSpeedByPitch(1.0f);
    }
}

void FXWindow::on_horizontalSlider_speed_by_pitch_valueChanged(int value)
{
    if (value == 100)
    {
        ui->label_speed_by_pitch->setText("Speed: x1");

        emit signalChangeSpeedByPitch(1.0f);

        ui->horizontalSlider_speed_by_time->setEnabled(true);
        ui->pushButton_speed_by_time->setEnabled(true);
    }
    else
    {
        float fSpeed = value / 100.0f;

        ui->label_speed_by_pitch->setText("Speed: x" + QString::number(static_cast<double>(fSpeed), 'f', 2));

        emit signalChangeSpeedByPitch(fSpeed);

        ui->horizontalSlider_speed_by_time->setEnabled(false);
        ui->pushButton_speed_by_time->setEnabled(false);
    }
}

void FXWindow::on_pushButton_speed_by_time_clicked()
{
    if (ui->horizontalSlider_speed_by_time->value() != 100)
    {
        ui->horizontalSlider_speed_by_time->setValue(100);
        ui->label_speed_by_time->setText("Speed: x1");

        emit signalChangeSpeedByTime(1.0f);
    }
}

void FXWindow::on_horizontalSlider_speed_by_time_valueChanged(int value)
{
    if (value == 100)
    {
        ui->label_speed_by_time->setText("Speed: x1");

        emit signalChangeSpeedByTime(1.0f);

        ui->horizontalSlider_speed_by_pitch->setEnabled(true);
        ui->pushButton_speed_by_pitch->setEnabled(true);
    }
    else
    {
        float fSpeed = value / 100.0f;

        ui->label_speed_by_time->setText("Speed: x" + QString::number(static_cast<double>(fSpeed), 'f', 2));

        emit signalChangeSpeedByTime(fSpeed);

        ui->horizontalSlider_speed_by_pitch->setEnabled(false);
        ui->pushButton_speed_by_pitch->setEnabled(false);
    }
}

void FXWindow::on_pushButton_reverb_clicked()
{
    ui->horizontalSlider_reverb->setValue(-80);
    ui->label_reverb->setText("Reverb wet volume: -80 dB");

    emit signalChangeReverbVolume(-80.0f);
}

void FXWindow::on_horizontalSlider_reverb_valueChanged(int value)
{
    ui->label_reverb->setText("Reverb wet volume: " + QString::number(value) + " dB");

    emit signalChangeReverbVolume(static_cast<float>(value));
}

void FXWindow::on_horizontalSlider_delay_valueChanged(int value)
{
    ui->label_delay->setText("Echo wet volume: " + QString::number(value) + " dB");

    emit signalChangeEchoVolume(static_cast<float>(value));
}

void FXWindow::on_pushButton_delay_clicked()
{
    ui->horizontalSlider_delay->setValue(-80);
    ui->label_delay->setText("Echo wet volume: -80 dB");

    emit signalChangeEchoVolume(-80.0f);
}

void FXWindow::on_pushButton_vst_clicked()
{
    if (bVSTLoaded)
    {
        emit signalShowVST();
    }
    else
    {
        QString pathToDll = QFileDialog::getOpenFileName(this, "Open file", "", "DLL (*.dll)");
        if (pathToDll != "")
        {
            wchar_t* pPath = new wchar_t[static_cast<size_t>(pathToDll.size() * 2) + 2];
            memset(pPath, 0, static_cast<size_t>(pathToDll.size() * 2) + 2);

            std::memcpy(pPath, pathToDll.toStdWString().c_str(), static_cast<size_t>(pathToDll.size() * 2));

            emit signalOpenVST(pPath);
        }
    }
}

void FXWindow::slotSetVSTName(QString name)
{
    if (name == "NULL")
    {
        ui->pushButton_vst->setText("Load your VST plugin");
        bVSTLoaded = false;
    }
    else
    {
        ui->pushButton_vst->setText(name);
        bVSTLoaded = true;
    }
}

void FXWindow::slotResetAll()
{
    on_pushButton_clicked();
}





void FXWindow::on_pushButton_clicked()
{
    on_pushButton_pan_clicked();
    on_pushButton_pitch_clicked();
    if (ui->pushButton_speed_by_pitch->isEnabled()) on_pushButton_speed_by_pitch_clicked();
    if (ui->pushButton_speed_by_time->isEnabled()) on_pushButton_speed_by_time_clicked();
    on_pushButton_reverb_clicked();
    on_pushButton_delay_clicked();
}




void FXWindow::closeEvent(QCloseEvent *ev)
{
    ev->ignore();
    hide();
}

FXWindow::~FXWindow()
{
    delete ui;
}
