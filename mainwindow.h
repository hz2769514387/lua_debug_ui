/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QListWidgetItem>
#include <QThread>
#include <QSharedMemory>
#include <QBuffer>
#include <QInputDialog>
#include <QComboBox>
#include <windows.h>

#include "mdichild.h"
#include "dialogfind.h"

class MdiChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QListWidget;
class QTextEdit;
QT_END_NAMESPACE



//自定义的Text，主要为了初始化大小
class MyTextWidget : public QTextEdit
{
public:
    MyTextWidget(QWidget *parent) : QTextEdit(parent)
    {
    }

    QSize sizeHint() const
    {
        return QSize(0, 80);
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();
    bool openFile(const QString &fileName);
    bool checkAndOpenFile( const QString &openFileName );
    void outPutConsole(const QString& msg)  {msgList->append(msg);}
    void outPutConsole(const char* msg)     {msgList->append(QString::fromLocal8Bit(msg));}

    void RefreshFuncList(const QList<func> *functionDetailList, bool bClearOldList);
    void refreshEncodingDisplay(int nEncoding);
    void exeuteFind(const QString &expr, bool re, bool cs, bool wo,bool wrap, bool insection, bool next);
    void exeuteReplace(const QString &exprFind,const QString &expr, bool re, bool cs, bool wo);
    void exeuteReplaceAll(const QString &exprFind,const QString &expr, bool re, bool cs, bool wo);
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *o, QEvent *e) Q_DECL_OVERRIDE;
    void timerEvent( QTimerEvent *event ) Q_DECL_OVERRIDE;
private slots:
    void newFile();
    void open();
    void save();
    void saveAs();
    void updateRecentFileActions();
    void openRecentFile();
#ifndef QT_NO_CLIPBOARD
    void cut();
    void copy();
    void paste();
#endif
    void about();
    void updateMenusAndTitle();
    void updateWindowMenu();
    MdiChild *createMdiChild();
    void showInExplorer();
    void funcListCliecked(QListWidgetItem *item);
    void findAndReplace();
    void findNext();
    void locateToLine();
private:
    enum { MaxRecentFiles = 5 };

    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool loadFile(const QString &fileName);
    static bool hasRecentFiles();
    void prependToRecentFiles(const QString &fileName);
    void setRecentFilesVisible(bool visible);
    MdiChild *activeMdiChild() const;
    QMdiSubWindow *findMdiChild(const QString &fileName) const;

    QMdiArea *mdiArea;

    QMenu *windowMenu;
    QAction *newAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *recentFileActs[MaxRecentFiles];
    QAction *recentFileSeparator;
    QAction *recentFileSubMenuAct;
#ifndef QT_NO_CLIPBOARD
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
#endif
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *windowMenuSeparatorAct;
    QAction *showInExplorerAct;
    QAction *findAct;
    QAction *findNextAct;
    QAction *targetToLineAct;

    QListWidget  *funcList;
    MyTextWidget *msgList;
    DialogFind   *findDlg;
    QComboBox    *cbEncoding;
    QComboBox    *cbEolMode;
    bool          bActiveWindow;
    int           timerShardMId;
    HANDLE        hSharedMMap;
    char*         pSharedMBuffer;
};


#endif
