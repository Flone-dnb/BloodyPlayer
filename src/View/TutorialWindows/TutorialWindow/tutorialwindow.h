#pragma once


#include <QMainWindow>

class QCloseEvent;

namespace Ui
{
    class TutorialWindow;
}


class TutorialWindow : public QMainWindow
{
    Q_OBJECT

signals:

    void close();
    void end();

public:

    explicit TutorialWindow(QWidget *parent = nullptr);

    ~TutorialWindow();

protected:

    void closeEvent(QCloseEvent* ev);

private slots:

    void on_pushButton_next_clicked();
    void on_pushButton_prev_clicked();

private:

    void changeSlide();



    Ui::TutorialWindow *ui;

    char cCurrentSlide;
};

