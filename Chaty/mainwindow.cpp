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


int groupNum = 1024;
void MainWindow::Receive(QTextBrowser* textBrowser) {
  while (true) {
      data.queueMutex.lock();
      puts("1");
      if(data.messages.empty()){
          QThread::msleep(300);
          puts("4");
      }
      else{
          puts("2");
          while(!data.messages.empty()){
              puts("3");
              Data::Datas buf = data.messages.front();
              QDateTime curDateTime = buf.time;
              textBrowser->append(QString("<font color=\"#AA6600\">") +
                                  QString(curDateTime.toString("yyyy-MM-dd hh:mm:ss")
                                              .toStdString()
                                              .c_str()) +
                                  QString("</font> "));
              char temp[2048];
              sprintf(temp, "Id : %20d\tMessage : %s", buf.data.id,buf.data.message);
              textBrowser->append(QString("<font color=\"#0066AA\">") + QString(temp) +
                                  QString("</font> "));
              textBrowser->append(QString(buf.data.data));
              textBrowser->insertPlainText("\n");
              data.messages.pop();
          }

          QTextCursor cursor = textBrowser->textCursor();
          cursor.movePosition(QTextCursor::End);
          textBrowser->setTextCursor(cursor);
      }
      data.queueMutex.unlock();
  }
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
    data.Chat(ui->textEdit->toPlainText().toStdString().c_str());
    ui->textEdit->clear();
  });


  receive = std::thread(&MainWindow::Receive,this,ui->textBrowser);
//  receive.detach();
}

MainWindow::~MainWindow() {
  delete SysIcon;
  delete icon;
  delete ui;
  QApplication::exit();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
      puts("SENDING");
    data.Chat(ui->textEdit->toPlainText().toStdString().c_str());
    ui->textEdit->clear();
  }
}

void MainWindow::on_listWidget_clicked(const QModelIndex& index) {
  ui->textBrowser->clear();
  if(this->groups.find(index.row()) == this->groups.end()){
      data.Connect(index.row());
      return;
  }
  for (auto i : data.GroupMessage(index.row())) {
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
