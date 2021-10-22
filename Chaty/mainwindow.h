#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "commondata.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QHostAddress>
#include <QUdpSocket>

#include <QObject>
#include <qsystemtrayicon.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QIcon *icon;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
    void closeEvent(QCloseEvent *event);
private slots:

    void on_listWidget_clicked(const QModelIndex &index);

    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

private:
    QSystemTrayIcon *SysIcon;
    void keyPressEvent(QKeyEvent *event);
    Ui::MainWindow *ui;
    QMenu *m_menu;
    QAction *m_action1;
    QAction *m_action2;
};
#endif // MAINWINDOW_H
