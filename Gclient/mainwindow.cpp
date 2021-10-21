#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QDateTime>
#include <QMouseEvent>
#include <QObject>
#include <QThread>
#include <Qt3DExtras/qt3dwindow.h>
#include <QTime>
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
        textBrowser->append(QString("<font color=\"#AA6600\">") + QString(curDateTime.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str()) + QString("</font> "));
        char temp[2048];
        sprintf(temp,"Group : %5d\tId : %20d\tMessage : %s",data.group,data.id,data.message);
        textBrowser->append(QString("<font color=\"#0066AA\">") + QString(temp) + QString("</font> "));
//        textBrowser->append(QString("<font color=\"#6600AA\">") + QString(data.data) + QString("</font> "));
        textBrowser->append(QString(data.data));
        textBrowser->insertPlainText("\n");
        textBrowser->moveCursor(QTextCursor::End);
    }
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
std::cout<<index.row()<<std::endl;
}


void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        data.code = CHAT;
        strcpy(data.data,ui->textEdit->toPlainText().toStdString().c_str());
        socket.writeDatagram((char*)&data,sizeof (CommonData), serverAddress,serverPort);
        ui->textEdit->clear();
    }
}
