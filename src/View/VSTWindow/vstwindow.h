#pragma once

#include <QMainWindow>

namespace Ui {
class VSTWindow;
}

class QCloseEvent;

class VSTWindow : public QMainWindow
{
    Q_OBJECT

signals:

    void unloadVST();
    void updateAudio();

public:

    explicit VSTWindow(QWidget *parent = nullptr);

    HWND getVSTWidgetHandle();

    ~VSTWindow();

protected:

    void closeEvent(QCloseEvent* ev);

private slots:

    void on_actionUnload_triggered();

private:

    Ui::VSTWindow *ui;
};
