/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPlainTextEdit *plainTextEdit;
    QGroupBox *groupBox;
    QDoubleSpinBox *doubleSpinBox;
    QLineEdit *lineEdit;
    QLabel *label;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox_2;
    QLCDNumber *lcdNumber;
    QLCDNumber *lcdNumber_2;
    QLabel *label_3;
    QGroupBox *groupBox_2;
    QDoubleSpinBox *doubleSpinBox_3;
    QLineEdit *lineEdit_2;
    QLabel *label_4;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBox_4;
    QLCDNumber *lcdNumber_3;
    QLCDNumber *lcdNumber_4;
    QLabel *label_6;
    QPushButton *pushButton_3;
    QPushButton *functionsButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(517, 417);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setToolButtonStyle(Qt::ToolButtonIconOnly);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(80, 320, 99, 27));
        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setEnabled(false);
        pushButton_2->setGeometry(QRect(190, 320, 99, 27));
        plainTextEdit = new QPlainTextEdit(centralWidget);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(520, 0, 291, 351));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setEnabled(true);
        groupBox->setGeometry(QRect(10, 0, 471, 131));
        groupBox->setAutoFillBackground(false);
        groupBox->setStyleSheet(QStringLiteral(""));
        groupBox->setFlat(false);
        doubleSpinBox = new QDoubleSpinBox(groupBox);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));
        doubleSpinBox->setGeometry(QRect(10, 90, 111, 27));
        doubleSpinBox->setMinimum(-1);
        doubleSpinBox->setMaximum(1);
        doubleSpinBox->setSingleStep(0.05);
        doubleSpinBox->setValue(1);
        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(50, 30, 411, 27));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 70, 101, 17));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(230, 70, 101, 17));
        doubleSpinBox_2 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_2->setObjectName(QStringLiteral("doubleSpinBox_2"));
        doubleSpinBox_2->setGeometry(QRect(230, 90, 121, 27));
        doubleSpinBox_2->setDecimals(4);
        doubleSpinBox_2->setMinimum(0);
        doubleSpinBox_2->setMaximum(100000);
        doubleSpinBox_2->setSingleStep(0.5);
        doubleSpinBox_2->setValue(500);
        lcdNumber = new QLCDNumber(groupBox);
        lcdNumber->setObjectName(QStringLiteral("lcdNumber"));
        lcdNumber->setGeometry(QRect(130, 70, 71, 51));
        lcdNumber->setSmallDecimalPoint(false);
        lcdNumber->setDigitCount(4);
        lcdNumber->setProperty("value", QVariant(0));
        lcdNumber_2 = new QLCDNumber(groupBox);
        lcdNumber_2->setObjectName(QStringLiteral("lcdNumber_2"));
        lcdNumber_2->setGeometry(QRect(360, 70, 101, 51));
        lcdNumber_2->setDigitCount(7);
        lcdNumber_2->setProperty("value", QVariant(0));
        lcdNumber_2->setProperty("intValue", QVariant(0));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 31, 31, 20));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setEnabled(true);
        groupBox_2->setGeometry(QRect(10, 150, 471, 131));
        groupBox_2->setAutoFillBackground(false);
        groupBox_2->setStyleSheet(QStringLiteral(""));
        groupBox_2->setFlat(false);
        doubleSpinBox_3 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_3->setObjectName(QStringLiteral("doubleSpinBox_3"));
        doubleSpinBox_3->setGeometry(QRect(10, 90, 111, 27));
        doubleSpinBox_3->setMinimum(-1);
        doubleSpinBox_3->setMaximum(1);
        doubleSpinBox_3->setSingleStep(0.05);
        doubleSpinBox_3->setValue(1);
        lineEdit_2 = new QLineEdit(groupBox_2);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(50, 30, 411, 27));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 70, 101, 17));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(230, 70, 101, 17));
        doubleSpinBox_4 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_4->setObjectName(QStringLiteral("doubleSpinBox_4"));
        doubleSpinBox_4->setGeometry(QRect(230, 90, 121, 27));
        doubleSpinBox_4->setDecimals(4);
        doubleSpinBox_4->setMinimum(0);
        doubleSpinBox_4->setMaximum(100000);
        doubleSpinBox_4->setSingleStep(0.5);
        doubleSpinBox_4->setValue(500);
        lcdNumber_3 = new QLCDNumber(groupBox_2);
        lcdNumber_3->setObjectName(QStringLiteral("lcdNumber_3"));
        lcdNumber_3->setGeometry(QRect(130, 70, 71, 51));
        lcdNumber_3->setSmallDecimalPoint(false);
        lcdNumber_3->setDigitCount(4);
        lcdNumber_3->setProperty("value", QVariant(0));
        lcdNumber_4 = new QLCDNumber(groupBox_2);
        lcdNumber_4->setObjectName(QStringLiteral("lcdNumber_4"));
        lcdNumber_4->setGeometry(QRect(360, 70, 101, 51));
        lcdNumber_4->setDigitCount(7);
        lcdNumber_4->setProperty("value", QVariant(0));
        lcdNumber_4->setProperty("intValue", QVariant(0));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 31, 31, 20));
        pushButton_3 = new QPushButton(centralWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setEnabled(false);
        pushButton_3->setGeometry(QRect(300, 320, 99, 27));
        functionsButton = new QPushButton(centralWidget);
        functionsButton->setObjectName(QStringLiteral("functionsButton"));
        functionsButton->setGeometry(QRect(490, 0, 21, 351));
        functionsButton->setCheckable(true);
        functionsButton->setChecked(false);
        functionsButton->setFlat(false);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 517, 25));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "SndGEN", 0));
        pushButton->setText(QApplication::translate("MainWindow", "\320\227\320\260\320\277\321\203\321\201\320\272", 0));
        pushButton_2->setText(QApplication::translate("MainWindow", "\320\241\321\202\320\276\320\277", 0));
        plainTextEdit->setPlainText(QApplication::translate("MainWindow", "double square(double t) {\n"
"  return sin(t) >=0 ? 1 : -1;\n"
"}", 0));
        groupBox->setTitle(QApplication::translate("MainWindow", "\320\233\320\265\320\262\321\213\320\271 \320\272\320\260\320\275\320\260\320\273", 0));
        lineEdit->setText(QApplication::translate("MainWindow", "sin(k*t)", 0));
        label->setText(QApplication::translate("MainWindow", "\320\220\320\274\320\277\320\273\320\270\321\202\321\203\320\264\320\260", 0));
        label_2->setText(QApplication::translate("MainWindow", "\320\247\320\260\321\201\321\202\320\276\321\202\320\260 (\320\223\321\206)", 0));
        label_3->setText(QApplication::translate("MainWindow", "f(t) =", 0));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "\320\237\321\200\320\260\320\262\321\213\320\271 \320\272\320\260\320\275\320\260\320\273", 0));
        lineEdit_2->setText(QApplication::translate("MainWindow", "cos(k*t)", 0));
        label_4->setText(QApplication::translate("MainWindow", "\320\220\320\274\320\277\320\273\320\270\321\202\321\203\320\264\320\260", 0));
        label_5->setText(QApplication::translate("MainWindow", "\320\247\320\260\321\201\321\202\320\276\321\202\320\260 (\320\223\321\206)", 0));
        label_6->setText(QApplication::translate("MainWindow", "f(t) =", 0));
        pushButton_3->setText(QApplication::translate("MainWindow", "\320\237\320\265\321\200\320\265\320\267\320\260\320\277\321\203\321\201\320\272", 0));
        functionsButton->setText(QApplication::translate("MainWindow", ">", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
