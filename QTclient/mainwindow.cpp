#include "mainwindow.h"
#include <QDateTime>
#include <QMouseEvent>
#include <QObject>
#include <QThread>
#include <Qt3DExtras/qt3dwindow.h>

#include "ui_mainwindow.h"
QUdpSocket socket;

void Receive(QTextBrowser *textBrowser){

    while(true){
        QHostAddress serverAddr;
        quint16 port;
        CommonData data;
        qint64 len = socket.readDatagram((char*)&data,sizeof (CommonData), &serverAddr, &port);
        if(len == -1){
            int ms = 300;
            struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
            nanosleep(&ts, NULL);
            continue;
        }
        QDateTime curDateTime=QDateTime::currentDateTime();
        textBrowser->insertPlainText(curDateTime.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str());
        textBrowser->insertPlainText("\tGroup : ");
        char temp[10];
        sprintf(temp,"%d",data.group);
        textBrowser->insertPlainText(temp);
        textBrowser->insertPlainText("\n");
        textBrowser->insertPlainText(data.message);
        textBrowser->insertPlainText("\n");
        textBrowser->insertPlainText(data.data);
        textBrowser->insertPlainText("\n");
        textBrowser->moveCursor(QTextCursor::End);
    }
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->menubar->addMenu("Login");


    ui->setupUi(this);
    receive = std::thread(Receive,ui->textBrowser);
    receive.detach();
    serverAddress = QHostAddress("172.22.251.189");
    serverPort = 9999;
    group = 0;
    this->groupSet.insert(group);

    CommonData data;
    memset(&data,0,sizeof (CommonData));
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
}


void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    std::cout<<"index : "<<index<<std::endl;
}
