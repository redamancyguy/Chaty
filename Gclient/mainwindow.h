#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "commondata.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QHostAddress>
#include <QUdpSocket>
#include <iostream>
#include <QThread>
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

private:
    Ui::MainWindow *ui;
    std::thread receive;

    CommonData data;

    QHostAddress serverAddress;
    quint16 serverPort;
    QSet<unsigned int> groupSet;

    void keyPressEvent(QKeyEvent *event);
};
#endif // MAINWINDOW_H
