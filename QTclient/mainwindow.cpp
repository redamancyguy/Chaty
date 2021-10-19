#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include <QMouseEvent>
//#include <QObject>

#include <Qt3DExtras/qt3dwindow.h>

#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    receive.start();
    receive.run();
    ui->menubar->addMenu("Login");
    serverAddress = QHostAddress("172.22.251.189");
    serverPort = 9999;

    group = 0;
    this->groupSet.insert(group);

    CommonData data;
    data.group = this->group;
    data.code = CONNECT;
    socket.writeDatagram((char*)&data,sizeof (CommonData), serverAddress,serverPort);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        this->on_pushButton_clicked();
    }
}

void MainWindow::on_pushButton_clicked()
{
    CommonData data;
    data.group = this->group;
    data.code = CHAT;
    strcpy(data.data,ui->textEdit->toPlainText().toStdString().c_str());
    socket.writeDatagram((char*)&data,sizeof (CommonData), serverAddress,serverPort);
    ui->textEdit->clear();
    ui->textBrowser->insertPlainText("New Massage !\n");
}
