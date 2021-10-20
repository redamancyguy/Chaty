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
    std::thread receive;

    QHostAddress serverAddress;
    quint16 serverPort;
    unsigned int group;
    QSet<unsigned int> groupSet;
    void keyPressEvent(QKeyEvent *event);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_comboBox_activated(const QString &arg1);

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
