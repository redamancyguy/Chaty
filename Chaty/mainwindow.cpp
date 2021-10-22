#include "mainwindow.h"
#include <Qt3DExtras/qt3dwindow.h>
#include <qmenu.h>
#include <qsystemtrayicon.h>
#include <QAction>
#include <QDateTime>
#include <QDockWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <QTime>
#include <QUdpSocket>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include "groupitem.h"
#include "ui_mainwindow.h"

void MainWindow::Receive(QTextBrowser* textBrowser) {
  while (true) {
    QHostAddress serverAddr;
    quint16 port;
    CommonData data;
    mutex.lock();
    qint64 len = socket.readDatagram((char*)&data, sizeof(CommonData),
                                     &serverAddr, &port);
    mutex.unlock();
    if (len == -1) {
      int ms = 300;
      struct timespec ts = {ms / 1000, (ms % 1000) * 1000 * 1000};
      nanosleep(&ts, NULL);
      continue;
    }
    if (data.group == dataBuf.group) {
      QDateTime curDateTime = QDateTime::currentDateTime();
      textBrowser->append(QString("<font color=\"#AA6600\">") +
                          QString(curDateTime.toString("yyyy-MM-dd hh:mm:ss")
                                      .toStdString()
                                      .c_str()) +
                          QString("</font> "));
      char temp[2048];
      sprintf(temp, "Id : %20d\tMessage : %s", data.id, data.message);
      textBrowser->append(QString("<font color=\"#0066AA\">") + QString(temp) +
                          QString("</font> "));
      textBrowser->append(QString(data.data));
      textBrowser->insertPlainText("\n");

      QTextCursor cursor = textBrowser->textCursor();
      cursor.movePosition(QTextCursor::End);
      textBrowser->setTextCursor(cursor);
    }
    DataTime temp;
    temp.data = data;
    temp.time = QDateTime::currentDateTime();
    groupMessage[data.group].push_back(temp);
  }
}

void MainWindow::Connect(unsigned int group) {
  dataBuf.group = group;
  memset(&dataBuf.message, 0, 64);
  memset(&dataBuf.data, 0, 1024);
  dataBuf.code = CONNECT;
  groupMessage[dataBuf.group] = std::vector<DataTime>();
  socket.writeDatagram((char*)&dataBuf, sizeof(CommonData), serverAddress,
                       serverPort);
}

void MainWindow::Disconnect(unsigned int group) {
  dataBuf.group = group;
  memset(&dataBuf.message, 0, 64);
  memset(&dataBuf.data, 0, 1024);
  dataBuf.code = DISCONNECT;
  groupMessage.erase(dataBuf.group);
  socket.writeDatagram((char*)&dataBuf, sizeof(CommonData), serverAddress,
                       serverPort);
}

void MainWindow::Chat(const char* data) {
  dataBuf.code = CHAT;
  strcpy(dataBuf.data, data);
  socket.writeDatagram((char*)&dataBuf, sizeof(CommonData), serverAddress,
                       serverPort);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  SysIcon = new QSystemTrayIcon(this);
  m_menu = new QMenu(this);
  m_action1 = new QAction(m_menu);
  m_action2 = new QAction(m_menu);

  m_action1->setText("Show Window");
  m_action2->setText("Exit");

  m_menu->addAction(m_action1);
  m_menu->addAction(m_action2);
  QApplication* app;

  connect(m_action1, &QAction::triggered, this, &MainWindow::show);
  connect(m_action2, &QAction::triggered, this, &app->exit);

  SysIcon->setContextMenu(m_menu);
  SysIcon->setToolTip(QObject::trUtf8("Chaty"));
  icon = new QIcon(":/chaty.ico");

  //  setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
  //  setFixedSize(this->width(), this->height());

  for (int i = 0; i < groupNum; i++) {
    char temp[20];
    sprintf(temp, "Group:%5d", i);
    //    ui->listWidget->insertItem(i, temp);
    auto item = new QListWidgetItem(ui->listWidget);
    auto wTemp = new groupitem();
    auto size = wTemp->sizeHint();
    size.setHeight(50);
    item->setSizeHint(size);
    wTemp->setName(temp);
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item, wTemp);
  }

  /// start ui setting
  ui->input_splitter->setStretchFactor(0, 90);
  ui->input_splitter->setStretchFactor(1, 10);
  ui->list_splitter->setStretchFactor(0, 10);
  ui->list_splitter->setStretchFactor(1, 90);
  /// fin ui setting

  connect(ui->send, &QPushButton::clicked, [&]() {
    Chat(ui->textEdit->toPlainText().toStdString().c_str());
    ui->textEdit->clear();
  });

  QTime time;
  time = QTime::currentTime();
  qsrand(time.msec() + time.second() * 1000);
  dataBuf.id = qrand();
  dataBuf.group = 0;

  socket.open(QIODevice::ReadWrite);
  serverAddress = QHostAddress("39.104.209.232");  // server address there
  serverPort = 9999;

  Connect(dataBuf.group);

  receive = std::thread(&MainWindow::Receive, this, ui->textBrowser);
  receive.detach();
}

MainWindow::~MainWindow() {
  mutex.lock();
  for (auto i : groupMessage) {
    memset(&dataBuf.message, 0, 64);
    memset(&dataBuf.data, 0, 1024);
    dataBuf.group = i.first;
    dataBuf.code = DISCONNECT;
    socket.writeDatagram((char*)&dataBuf, sizeof(CommonData), serverAddress,
                         serverPort);
  }
  socket.close();
  mutex.unlock();
  delete SysIcon;
  delete icon;
  delete ui;
  QApplication::exit();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
    Chat(ui->textEdit->toPlainText().toStdString().c_str());
    ui->textEdit->clear();
  }
}

void MainWindow::on_listWidget_clicked(const QModelIndex& index) {
  ui->textBrowser->clear();
  if (groupMessage.find(index.row()) == groupMessage.end()) {
    mutex.lock();
    groupMessage[index.row()] = std::vector<DataTime>();
    Connect(index.row());
    mutex.unlock();
    return;
  }
  for (auto i : groupMessage[index.row()]) {
    QDateTime curDateTime = i.time;
    ui->textBrowser->append(
        QString("<font color=\"#AA6600\">") +
        QString(
            curDateTime.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str()) +
        QString("</font> "));
    char temp[2048];
    sprintf(temp, "Id : %20d\tMessage : %s", i.data.id, i.data.message);
    ui->textBrowser->append(QString("<font color=\"#0066AA\">") +
                            QString(temp) + QString("</font> "));
    ui->textBrowser->append(QString(i.data.data));
    ui->textBrowser->insertPlainText("\n");
  }
  QTextCursor cursor = ui->textBrowser->textCursor();
  cursor.movePosition(QTextCursor::End);
  ui->textBrowser->setTextCursor(cursor);
}

void MainWindow::closeEvent(QCloseEvent* event) {
  event->ignore();
  SysIcon->setIcon(*icon);
  SysIcon->show();
  this->hide();
}
