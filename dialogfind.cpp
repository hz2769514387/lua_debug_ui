#include "dialogfind.h"
#include "ui_dialogfind.h"
#include "mainwindow.h"

DialogFind::DialogFind(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFind)
{
    ui->setupUi(this);
    parentMain = NULL;
    ui->comboBoxFind->setInsertPolicy(QComboBox::InsertAtTop);
    ui->comboBoxFind->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);
    ui->comboBoxReplace->setInsertPolicy(QComboBox::InsertAtTop);
    ui->comboBoxReplace->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);
    ui->checkBoxCS->setChecked(true);
}

DialogFind::~DialogFind()
{
    delete ui;
}

void DialogFind::saveFindItem(const QString& itemText)
{
    if(itemText != NULL && !itemText.isEmpty())
    {
        if(ui->comboBoxFind->findText(itemText) >= 0)
        {
            ui->comboBoxFind->setCurrentText(itemText);
        }
        else
        {
            ui->comboBoxFind->insertItem(0,itemText);
            ui->comboBoxFind->setCurrentIndex(0);
        }
    }
}

void DialogFind::saveReplaceItem(const QString& itemText)
{
    if(itemText != NULL && !itemText.isEmpty())
    {
        if(ui->comboBoxReplace->findText(itemText) >= 0)
        {
            ui->comboBoxReplace->setCurrentText(itemText);
        }
        else
        {
            ui->comboBoxReplace->insertItem(0,itemText);
            ui->comboBoxReplace->setCurrentIndex(0);
        }
    }
}

void DialogFind::showWithInsertItem(const QString& itemText)
{
    saveFindItem(itemText);
    this->showNormal();
}

void DialogFind::on_pushButtonPrev_clicked()
{
    saveFindItem(ui->comboBoxFind->currentText());
    saveReplaceItem(ui->comboBoxReplace->currentText());
    if(parentMain)
    {
        parentMain->exeuteFind(this->ui->comboBoxFind->currentText(),
                               ui->checkBoxRE->isChecked(),
                               ui->checkBoxCS->isChecked(),
                               ui->checkBoxWO->isChecked(),
                               true,
                               ui->checkBoxSEL->isChecked(),
                               false);
    }
}

void DialogFind::on_pushButtonNext_clicked()
{
    saveFindItem(ui->comboBoxFind->currentText());
    saveReplaceItem(ui->comboBoxReplace->currentText());
    if(parentMain)
    {
        parentMain->exeuteFind(this->ui->comboBoxFind->currentText(),
                               ui->checkBoxRE->isChecked(),
                               ui->checkBoxCS->isChecked(),
                               ui->checkBoxWO->isChecked(),
                               true,
                               ui->checkBoxSEL->isChecked(),
                               true);
    }
}

void DialogFind::on_pushButtonReplace_clicked()
{
    saveFindItem(ui->comboBoxFind->currentText());
    saveReplaceItem(ui->comboBoxReplace->currentText());
    if(parentMain)
    {
        parentMain->exeuteReplace(this->ui->comboBoxFind->currentText(),
                                  this->ui->comboBoxReplace->currentText(),
                                  ui->checkBoxRE->isChecked(),
                                  ui->checkBoxCS->isChecked(),
                                  ui->checkBoxWO->isChecked(),
                                  true,
                                  ui->checkBoxSEL->isChecked(),
                                  true);
    }
}

void DialogFind::on_pushButtonREall_clicked()
{
    saveFindItem(ui->comboBoxFind->currentText());
    saveReplaceItem(ui->comboBoxReplace->currentText());
    if(parentMain)
    {
        parentMain->exeuteReplaceAll(this->ui->comboBoxFind->currentText(),
                                  this->ui->comboBoxReplace->currentText(),
                                  ui->checkBoxRE->isChecked(),
                                  ui->checkBoxCS->isChecked(),
                                  ui->checkBoxWO->isChecked());
    }
}
