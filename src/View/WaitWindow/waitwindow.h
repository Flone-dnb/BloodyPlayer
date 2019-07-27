#pragma once

#include <QMainWindow>

namespace Ui {
class WaitWindow;
}

class WaitWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit WaitWindow(QWidget *parent = nullptr);

    void setLabelText(QString text);

    void setProgressValue(int value);
    void setProgressMax(int value);

    ~WaitWindow();


private:

    Ui::WaitWindow *ui;
};
