#ifndef DIALOGFIND_H
#define DIALOGFIND_H

#include <QDialog>

namespace Ui {
class DialogFind;
}

class MainWindow;
class DialogFind : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFind(QWidget *parent = 0);
    ~DialogFind();
    void InitParentMain(MainWindow* mainWindowParent){parentMain = mainWindowParent;}
    void showWithInsertItem(const QString& itemText);
    void saveFindItem(const QString& itemText);
    void saveReplaceItem(const QString& itemText);
private slots:
    void on_pushButtonPrev_clicked();
    void on_pushButtonNext_clicked();
    void on_pushButtonReplace_clicked();

    void on_pushButtonREall_clicked();

private:
    Ui::DialogFind *ui;
    MainWindow     *parentMain;
};

#endif // DIALOGFIND_H
