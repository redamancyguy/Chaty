#include "mainwindow.h"
#include <QDateTime>
#include <QMouseEvent>
#include <QObject>
#include <QThread>
#include <Qt3DExtras/qt3dwindow.h>
#include <QMutex>
#include <QTime>
#include "ui_mainwindow.h"
#include <iostream>
#include <queue>
#include <vector>
#include <unordered_map>
#include <QThread>
#include <qsystemtrayicon.h>
#include <QMessageBox>
#include <QAction>
#include <qmenu.h>


QUdpSocket socket;
QHostAddress serverAddress;
quint16 serverPort;

std::thread receive;
QMutex mutex;

int groupNum = 1024;
CommonData dataBuf;
struct DataTime{
    QDateTime time;
    CommonData data;
};
std::unordered_map<unsigned int,std::vector<DataTime>> groupMessage;

void Receive(QTextBrowser *textBrowser){
    while(true){
        QHostAddress serverAddr;
        quint16 port;
        CommonData data;
        mutex.lock();
        qint64 len = socket.readDatagram((char*)&data,sizeof (CommonData), &serverAddr, &port);
        mutex.unlock();
        if(len == -1){
            int ms = 300;
            struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
            nanosleep(&ts, NULL);
            continue;
        }
        if(data.group == dataBuf.group){
            QDateTime curDateTime=QDateTime::currentDateTime();
            textBrowser->append(QString("<font color=\"#AA6600\">") + QString(curDateTime.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str()) + QString("</font> "));
            char temp[2048];
            sprintf(temp,"Id : %20d\tMessage : %s",data.id,data.message);
            textBrowser->append(QString("<font color=\"#0066AA\">") + QString(temp) + QString("</font> "));
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

void Connect(unsigned int group){
    dataBuf.group = group;
    memset(&dataBuf.message,0,64);
    memset(&dataBuf.data,0,1024);
    dataBuf.code = CONNECT;
    groupMessage[dataBuf.group] = std::vector<DataTime>();
    socket.writeDatagram((char*)&dataBuf,sizeof (CommonData), serverAddress,serverPort);
}


void Disconnect(unsigned int group){
    dataBuf.group = group;
    memset(&dataBuf.message,0,64);
    memset(&dataBuf.data,0,1024);
    dataBuf.code = DISCONNECT;
    groupMessage.erase(dataBuf.group);
    socket.writeDatagram((char*)&dataBuf,sizeof (CommonData), serverAddress,serverPort);
}


void Chat(const char *data){
    dataBuf.code = CHAT;
    strcpy(dataBuf.data,data);
    socket.writeDatagram((char*)&dataBuf,sizeof (CommonData), serverAddress,serverPort);
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SysIcon = new QSystemTrayIcon(this);
    connect(SysIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
    SysIcon->setToolTip(QObject::trUtf8("Chaty"));
    icon = new  QIcon("C:\\Users\\sunwenli\\Documents\\GitHub\\Chaty\\Chaty\\chaty.ico");

    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());

    for(int i=0;i<groupNum;i++){
        char temp[20];
        sprintf(temp,"Group:%5d",i);
        ui->listWidget->insertItem(i,temp);
    }

    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    dataBuf.id = qrand();
    dataBuf.group = 0;

    socket.open(QIODevice::ReadWrite);
    serverAddress = QHostAddress("39.104.209.232"); //server address there
    serverPort = 9999;

    Connect(dataBuf.group);

    receive = std::thread(Receive,ui->textBrowser);
    receive.detach();
}

MainWindow::~MainWindow()
{
    mutex.lock();
    for(auto i:groupMessage){
        memset(&dataBuf.message,0,64);
        memset(&dataBuf.data,0,1024);
        dataBuf.group = i.first;
        dataBuf.code = DISCONNECT;
        socket.writeDatagram((char*)&dataBuf,sizeof (CommonData), serverAddress,serverPort);
    }
    socket.close();
    mutex.unlock();
    delete SysIcon;
    delete icon;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        Chat(ui->textEdit->toPlainText().toStdString().c_str());
        ui->textEdit->clear();
    }
}


void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    ui->textBrowser->clear();
    if(groupMessage.find(index.row()) == groupMessage.end()){
        mutex.lock();
        groupMessage[index.row()] = std::vector<DataTime>();
        Connect(index.row());
        mutex.unlock();
        return;
    }
    for(auto i:groupMessage[index.row()]){
        QDateTime curDateTime = i.time;
        ui->textBrowser->append(QString("<font color=\"#AA6600\">") + QString(curDateTime.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str()) + QString("</font> "));
        char temp[2048];
        sprintf(temp,"Id : %20d\tMessage : %s",i.data.id,i.data.message);
        ui->textBrowser->append(QString("<font color=\"#0066AA\">") + QString(temp) + QString("</font> "));
        ui->textBrowser->append(QString(i.data.data));
        ui->textBrowser->insertPlainText("\n");
    }
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textBrowser->setTextCursor(cursor);
}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason){
    if (reason == QSystemTrayIcon::Trigger){
        this->show();
        SysIcon->hide();
    }else if(reason == QSystemTrayIcon::Context){
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
    }
}

void MainWindow::closeEvent(QCloseEvent *event){
    event->ignore();
    SysIcon->setIcon(*icon);
    SysIcon->show();
    this->hide();
}

