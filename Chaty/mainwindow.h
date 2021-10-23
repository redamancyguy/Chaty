#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qsystemtrayicon.h>
#include <QByteArray>
#include <QCoreApplication>
#include <QDateTime>
#include <QHostAddress>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMutex>
#include <QObject>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QTextBrowser>
#include <QThread>
#include <QUdpSocket>
#include <unordered_map>
#include "commondata.h"
#include "data.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  QIcon* icon;
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
  void Receive(QTextBrowser* textBrowser);

 protected slots:
  void closeEvent(QCloseEvent* event);
 private slots:

  void on_listWidget_clicked(const QModelIndex& index);

 private:
  QSystemTrayIcon* SysIcon;
  void keyPressEvent(QKeyEvent* event);
  Ui::MainWindow* ui;
  QMenu* m_menu;
  QAction* m_action1;
  QAction* m_action2;

  Data data;
  std::thread receive;

  QSet<unsigned> groups;

};
#endif  // MAINWINDOW_H
