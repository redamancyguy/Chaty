#ifndef MYTHREAD_H
#define MYTHREAD_H



#include <QThread>
#include <QObject>
#include "ui_mainwindow.h"
class MyThread : public QObject
{
    Q_OBJECT
public:
    QTextBrowser *textBrowser;
    MyThread();
    ~MyThread();
    void run();
    void start();
    void stop();
signals:
    void mySignal();
private:
    bool isrun;
public slots:
};

#endif // MYTHREAD_H
