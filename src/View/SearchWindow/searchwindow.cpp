#include "searchwindow.h"
#include "ui_searchwindow.h"

// Qt
#include <QCloseEvent>
#include <QKeyEvent>

SearchWindow::SearchWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchWindow)
{
    ui->setupUi(this);

    setFixedSize( width(), height() );
}



void SearchWindow::closeEvent(QCloseEvent *ev)
{
    deleteLater();
}



void SearchWindow::on_pushButton_clicked()
{
    emit findPrev();
}


void SearchWindow::on_pushButton_2_clicked()
{
    emit findNext();
}

void SearchWindow::on_lineEdit_textChanged(const QString &arg1)
{
    emit searchTextChanged (arg1);
}

void SearchWindow::searchMatchCount(size_t iCount)
{
    if (iCount == 0)
    {
        ui->label_matches->setText("No matches.");
    }
    else
    {
        ui->label_matches->setText("Found matches: " + QString::number(iCount) + ".");
    }
}

void SearchWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Return)
    {
        findNext();
    }
    else if (ev->key() == Qt::Key_Escape)
    {
        close();
    }
}

SearchWindow::~SearchWindow()
{
    delete ui;
}
