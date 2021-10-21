/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QListWidget *listWidget;
    QTextBrowser *textBrowser;
    QTextEdit *textEdit;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(662, 378);
        MainWindow->setMinimumSize(QSize(662, 378));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(0, 20, 91, 311));
        listWidget->setStyleSheet(QString::fromUtf8("QWidget{\n"
"	font:9pt \"Arial Black\";\n"
"	font-family: Source Sans Pro; \n"
"    font-weight: bold; \n"
"    background-color:rgb(214,214,214);\n"
"    color:rgb(255,0,100);\n"
"}"));
        textBrowser = new QTextBrowser(centralwidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(90, 20, 571, 201));
        textBrowser->setStyleSheet(QString::fromUtf8("QWidget{\n"
"	color:rgb(150,0,150);\n"
"	font:12pt \"Arial Black\";\n"
"    background-color:rgb(255,225,225);\n"
"	font-family: Source Sans Pro; \n"
"    font-weight: bold; \n"
"}"));
        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(90, 220, 571, 111));
        textEdit->setStyleSheet(QString::fromUtf8("QWidget{\n"
"	font:12pt \"Arial Black\";\n"
"	font-family: Source Sans Pro; \n"
"    font-weight: bold; \n"
"    background-color:beige;\n"
"    color:rgb(150,0,150);\n"
"}\n"
""));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 662, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
        for(int i=0;i<1024;i++){
                    char temp[20];
                    sprintf(temp,"Group:%5d",i);
                    listWidget->insertItem(i,temp);
                }
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
