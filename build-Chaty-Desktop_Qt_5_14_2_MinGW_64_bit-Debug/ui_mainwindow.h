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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_4;
    QSplitter *list_splitter;
    QListWidget *listWidget;
    QSplitter *input_splitter;
    QTextBrowser *textBrowser;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *send;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1033, 704);
        MainWindow->setMinimumSize(QSize(800, 413));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/chaty.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow{\n"
"	background-color: rgb(255, 255, 255);\n"
"}\n"
"font: 9pt \"Arial\";"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout_4 = new QHBoxLayout(centralwidget);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        list_splitter = new QSplitter(centralwidget);
        list_splitter->setObjectName(QString::fromUtf8("list_splitter"));
        list_splitter->setOrientation(Qt::Horizontal);
        list_splitter->setChildrenCollapsible(false);
        listWidget = new QListWidget(list_splitter);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
        listWidget->setSizePolicy(sizePolicy);
        listWidget->setStyleSheet(QString::fromUtf8(""));
        list_splitter->addWidget(listWidget);
        input_splitter = new QSplitter(list_splitter);
        input_splitter->setObjectName(QString::fromUtf8("input_splitter"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(input_splitter->sizePolicy().hasHeightForWidth());
        input_splitter->setSizePolicy(sizePolicy1);
        input_splitter->setOrientation(Qt::Vertical);
        input_splitter->setChildrenCollapsible(false);
        textBrowser = new QTextBrowser(input_splitter);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(textBrowser->sizePolicy().hasHeightForWidth());
        textBrowser->setSizePolicy(sizePolicy2);
        textBrowser->setStyleSheet(QString::fromUtf8("QWidget{\n"
"	color:rgb(150,0,150);\n"
"	font:12pt \"Arial Black\";\n"
"    background-color:#F5F6F7;\n"
"	font-family: Source Sans Pro; \n"
"    font-weight: bold; \n"
"	border:none;\n"
"}\n"
""));
        input_splitter->addWidget(textBrowser);
        widget = new QWidget(input_splitter);
        widget->setObjectName(QString::fromUtf8("widget"));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        textEdit = new QTextEdit(widget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        sizePolicy2.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
        textEdit->setSizePolicy(sizePolicy2);
        QFont font;
        font.setPointSize(12);
        textEdit->setFont(font);
        textEdit->setStyleSheet(QString::fromUtf8("QTextEdit{\n"
"	border:none;\n"
"}"));

        verticalLayout->addWidget(textEdit);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        send = new QPushButton(widget);
        send->setObjectName(QString::fromUtf8("send"));
        QFont font1;
        font1.setPointSize(10);
        send->setFont(font1);
        send->setAutoFillBackground(false);
        send->setStyleSheet(QString::fromUtf8("QPushButton{\n"
"	margin:5px;\n"
"	height:30px;\n"
"	color: rgb(30, 111, 255);\n"
"	width:80px;\n"
"}"));
        send->setAutoDefault(false);
        send->setFlat(false);

        horizontalLayout_3->addWidget(send);


        verticalLayout->addLayout(horizontalLayout_3);

        input_splitter->addWidget(widget);
        list_splitter->addWidget(input_splitter);

        horizontalLayout_4->addWidget(list_splitter);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        send->setDefault(false);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Chaty", nullptr));
        textBrowser->setHtml(QCoreApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Source Sans Pro'; font-size:12pt; font-weight:600; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'SimSun'; font-size:15pt; font-weight:400;\"><br /></p></body></html>", nullptr));
        send->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
