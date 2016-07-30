/********************************************************************************
** Form generated from reading UI file 'dialogfind.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGFIND_H
#define UI_DIALOGFIND_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_DialogFind
{
public:
    QFrame *line;
    QLabel *label;
    QComboBox *comboBoxFind;
    QCheckBox *checkBoxCS;
    QCheckBox *checkBoxRE;
    QCheckBox *checkBoxWO;
    QCheckBox *checkBoxSEL;
    QComboBox *comboBoxReplace;
    QLabel *labelReplace;
    QPushButton *pushButtonNext;
    QPushButton *pushButtonPrev;
    QPushButton *pushButtonReplace;
    QPushButton *pushButtonREall;

    void setupUi(QDialog *DialogFind)
    {
        if (DialogFind->objectName().isEmpty())
            DialogFind->setObjectName(QStringLiteral("DialogFind"));
        DialogFind->resize(408, 268);
        line = new QFrame(DialogFind);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(0, 150, 391, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        label = new QLabel(DialogFind);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 20, 54, 12));
        comboBoxFind = new QComboBox(DialogFind);
        comboBoxFind->setObjectName(QStringLiteral("comboBoxFind"));
        comboBoxFind->setGeometry(QRect(10, 40, 281, 21));
        comboBoxFind->setEditable(true);
        checkBoxCS = new QCheckBox(DialogFind);
        checkBoxCS->setObjectName(QStringLiteral("checkBoxCS"));
        checkBoxCS->setGeometry(QRect(10, 70, 181, 16));
        checkBoxRE = new QCheckBox(DialogFind);
        checkBoxRE->setObjectName(QStringLiteral("checkBoxRE"));
        checkBoxRE->setGeometry(QRect(10, 90, 181, 16));
        checkBoxWO = new QCheckBox(DialogFind);
        checkBoxWO->setObjectName(QStringLiteral("checkBoxWO"));
        checkBoxWO->setGeometry(QRect(10, 110, 181, 16));
        checkBoxSEL = new QCheckBox(DialogFind);
        checkBoxSEL->setObjectName(QStringLiteral("checkBoxSEL"));
        checkBoxSEL->setGeometry(QRect(10, 130, 181, 16));
        comboBoxReplace = new QComboBox(DialogFind);
        comboBoxReplace->setObjectName(QStringLiteral("comboBoxReplace"));
        comboBoxReplace->setGeometry(QRect(10, 200, 281, 21));
        comboBoxReplace->setEditable(true);
        labelReplace = new QLabel(DialogFind);
        labelReplace->setObjectName(QStringLiteral("labelReplace"));
        labelReplace->setGeometry(QRect(10, 180, 54, 12));
        pushButtonNext = new QPushButton(DialogFind);
        pushButtonNext->setObjectName(QStringLiteral("pushButtonNext"));
        pushButtonNext->setGeometry(QRect(304, 40, 91, 51));
        pushButtonPrev = new QPushButton(DialogFind);
        pushButtonPrev->setObjectName(QStringLiteral("pushButtonPrev"));
        pushButtonPrev->setGeometry(QRect(304, 10, 91, 23));
        pushButtonReplace = new QPushButton(DialogFind);
        pushButtonReplace->setObjectName(QStringLiteral("pushButtonReplace"));
        pushButtonReplace->setGeometry(QRect(304, 170, 91, 51));
        pushButtonREall = new QPushButton(DialogFind);
        pushButtonREall->setObjectName(QStringLiteral("pushButtonREall"));
        pushButtonREall->setGeometry(QRect(304, 230, 91, 23));

        retranslateUi(DialogFind);

        QMetaObject::connectSlotsByName(DialogFind);
    } // setupUi

    void retranslateUi(QDialog *DialogFind)
    {
        DialogFind->setWindowTitle(QApplication::translate("DialogFind", "Find And Replace", 0));
        label->setText(QApplication::translate("DialogFind", "Find:", 0));
        checkBoxCS->setText(QApplication::translate("DialogFind", "Case sensitive(&C)", 0));
        checkBoxRE->setText(QApplication::translate("DialogFind", "As a regular expression(&E)", 0));
        checkBoxWO->setText(QApplication::translate("DialogFind", "Whole word matches only(&M)", 0));
        checkBoxSEL->setText(QApplication::translate("DialogFind", "Find in selection(&S)", 0));
        labelReplace->setText(QApplication::translate("DialogFind", "Replace:", 0));
        pushButtonNext->setText(QApplication::translate("DialogFind", "Find Next(&N)", 0));
        pushButtonPrev->setText(QApplication::translate("DialogFind", "Find Prev(&P)", 0));
        pushButtonReplace->setText(QApplication::translate("DialogFind", "Replace(&R)", 0));
        pushButtonREall->setText(QApplication::translate("DialogFind", "ReplaceAll(&A)", 0));
    } // retranslateUi

};

namespace Ui {
    class DialogFind: public Ui_DialogFind {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGFIND_H
