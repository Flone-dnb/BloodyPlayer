#pragma once


#include <QMainWindow>


class QCloseEvent;
class QKeyEvent;

namespace Ui
{
    class SearchWindow;
}

class SearchWindow : public QMainWindow
{
    Q_OBJECT

signals:

    void findNext ();
    void findPrev ();
    void searchTextChanged (QString keyword);

public:

    explicit SearchWindow(QWidget *parent = nullptr);

    ~SearchWindow();

public slots:

    void searchMatchCount (size_t iCount);

protected:

    void keyPressEvent  (QKeyEvent* ev);
    void closeEvent     (QCloseEvent* ev);

private slots:

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:

    Ui::SearchWindow *ui;
};
