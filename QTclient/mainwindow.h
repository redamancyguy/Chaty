#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "commondata.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QHostAddress>
#include <QUdpSocket>

#include <QObject>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:

    void on_listWidget_clicked(const QModelIndex &index);

    void on_listWidget_currentRowChanged(int currentRow);

private:
    void keyPressEvent(QKeyEvent *event);
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
