#include "mainwindow.h"
#include "profile.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Chaty");
    w.show();
    w.setWindowIcon(*w.icon);
    profile p;
    p.show();
    return a.exec();
}
