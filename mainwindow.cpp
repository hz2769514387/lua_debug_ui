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

#include <QtWidgets>
#include "mainwindow.h"
#define SHARED_MEM_NAME  L"LUA_DEBUG_UI_SHARED_OPENFILE"

MainWindow::~MainWindow()
{
    if ( timerShardMId != 0 )
    {
        killTimer(timerShardMId);
    }
    if(pSharedMBuffer)
    {
         ::UnmapViewOfFile(pSharedMBuffer);
        pSharedMBuffer = NULL;
    }
    if(hSharedMMap)
    {
        ::CloseHandle(hSharedMMap);
        hSharedMMap = NULL;
    }
}

MainWindow::MainWindow()
    : mdiArea(new QMdiArea)
{
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //tab模式显示
    mdiArea->setViewMode(QMdiArea::TabbedView);
    mdiArea->setTabsClosable(true);
    mdiArea->setTabsMovable(true);

    setCentralWidget(mdiArea);
    connect(mdiArea, &QMdiArea::subWindowActivated,this, &MainWindow::updateMenusAndTitle);

    createActions();
    createStatusBar();
    updateMenusAndTitle();

    readSettings();

    setWindowTitle(tr("lua_debug_ui"));
    setUnifiedTitleAndToolBarOnMac(true);

    //设定tab的右键菜单
    foreach (QTabBar* tab, mdiArea->findChildren<QTabBar *>())
    {
        tab->addAction(closeAct);
        tab->addAction(closeAllAct);
        tab->addAction(saveAct);
        tab->addAction(saveAsAct);

        QAction* sepAction = new QAction(tr("---------------------"), this);
        sepAction->setEnabled(false);
        tab->addAction(sepAction);
        tab->addAction(showInExplorerAct);
        tab->setContextMenuPolicy(Qt::ActionsContextMenu);

        tab->setExpanding(false);
    }

    //函数列表
    QDockWidget * dock = new QDockWidget(tr("Functions"), this);
    funcList = new QListWidget(dock);
    funcList->setAcceptDrops(false);
    dock->setWidget(funcList);
    //dock->setFeatures(QDockWidget::DockWidgetMovable );
    addDockWidget(Qt::RightDockWidgetArea, dock);
    connect(funcList, &QListWidget::itemDoubleClicked,this, &MainWindow::funcListCliecked);

    //输出窗口
    dock = new QDockWidget(tr("Console"), this);
    msgList = new MyTextWidget(dock);
    msgList->setAcceptDrops(false);
    dock->setWidget(msgList);
    //dock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::BottomDockWidgetArea, dock);

    //只有主窗口允许拖曳
    setAcceptDrops(true);

    //最大化
    setWindowState(Qt::WindowMaximized);

    //设置应用程序图标
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/images/app.ico")), QIcon::Normal, QIcon::Off);
    setWindowIcon(icon);

    //find窗口
    findDlg = new DialogFind(this);
    findDlg->InitParentMain(this);

    //label调色板定义
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::blue);

    //文件编码显示
    QLabel *textEncoding = new QLabel(this);
    textEncoding->setText("Encoding:");
    textEncoding->setPalette(pe);
    statusBar()->addPermanentWidget(textEncoding);
    cbEncoding = new QComboBox(this);
    cbEncoding->insertItem(0,"ANSI");
    cbEncoding->insertItem(1,"Unicode");
    cbEncoding->insertItem(2,"Unicode big endian");
    cbEncoding->insertItem(3,"UTF-8");
    statusBar()->addPermanentWidget(cbEncoding);

    //EOL 模式
    QLabel *textEolMode = new QLabel(this);
    textEolMode->setText("    EOL Mode:");
    textEolMode->setPalette(pe);
    statusBar()->addPermanentWidget(textEolMode);
    cbEolMode = new QComboBox(this);
    cbEolMode->insertItem(0,"Windows Mode (CRLF)");
    cbEolMode->insertItem(1,"Linux Mode (CR)");
    cbEolMode->insertItem(2,"Mac Mode(LF)");
    statusBar()->addPermanentWidget(cbEolMode);

    //安装事件过滤器
    QWidget::installEventFilter(this);

    //启动定时器
    bActiveWindow = true;
    hSharedMMap = NULL;
    pSharedMBuffer = NULL;
    timerShardMId = startTimer(100);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        openFile(fileName);
}

bool MainWindow::openFile(const QString &fileName)
{

    if (QMdiSubWindow *existing = findMdiChild(fileName))
    {
        mdiArea->setActiveSubWindow(existing);
        return true;
    }

    const bool succeeded = loadFile(fileName);
    if (succeeded)
        statusBar()->showMessage(tr("File loaded"), 2000);
    return succeeded;
}

bool MainWindow::loadFile(const QString &fileName)
{
    MdiChild *child = createMdiChild();
    const bool succeeded = child->loadFile(fileName);
    if (succeeded)
    {
        child->showMaximized();
    }
    else
    {
        child->close();
    }
    MainWindow::prependToRecentFiles(fileName);
    return succeeded;
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

bool MainWindow::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

void MainWindow::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void MainWindow::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        openFile(action->data().toString());
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    MdiChild *child = activeMdiChild();
    if (child && child->saveAs()) {
        statusBar()->showMessage(tr("File saved"), 2000);
        MainWindow::prependToRecentFiles(child->currentFile());
    }
}

#ifndef QT_NO_CLIPBOARD
void MainWindow::cut()
{
    if (activeMdiChild())
        activeMdiChild()->cut();
}

void MainWindow::copy()
{
    if (activeMdiChild())
        activeMdiChild()->copy();
}

void MainWindow::paste()
{
    if (activeMdiChild())
        activeMdiChild()->paste();
}
#endif

void MainWindow::about()
{
   QMessageBox::about(this, tr("About lua_debug_ui"),
            tr("The <b>lua_debug_ui</b> example demonstrates how to write multiple "
               "document interface applications using Qt."));
}

void MainWindow::updateMenusAndTitle()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
#ifndef QT_NO_CLIPBOARD
    pasteAct->setEnabled(hasMdiChild);
#endif
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    windowMenuSeparatorAct->setVisible(hasMdiChild);
    showInExplorerAct->setEnabled(hasMdiChild);
    findAct->setEnabled(hasMdiChild);
    findNextAct->setEnabled(hasMdiChild);
    targetToLineAct->setEnabled(hasMdiChild);
#ifndef QT_NO_CLIPBOARD
    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild() ->hasSelectedText());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
#endif

    //将当前文件路径写在标题栏上
    if(hasMdiChild)
    {
        setWindowTitle(activeMdiChild()->currentFile());
    }
}

class ActiveMdiSubWindowFunctor {
public:
    explicit ActiveMdiSubWindowFunctor(QMdiArea *mdiArea, QMdiSubWindow *activeWindow) : m_mdiArea(mdiArea), m_activeWindow(activeWindow) {}
    void operator()() const { m_mdiArea->setActiveSubWindow(m_activeWindow); }

private:
    QMdiArea *m_mdiArea;
    QMdiSubWindow *m_activeWindow;
};

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(windowMenuSeparatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    windowMenuSeparatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);
        MdiChild *child = qobject_cast<MdiChild *>(mdiSubWindow->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action = windowMenu->addAction(text, mdiSubWindow, ActiveMdiSubWindowFunctor(mdiArea, mdiSubWindow));
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
    }
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild(*this);
    mdiArea->addSubWindow(child);

#ifndef QT_NO_CLIPBOARD
    connect(child, &MdiChild::copyAvailable, cutAct, &QAction::setEnabled);
    connect(child, &MdiChild::copyAvailable, copyAct, &QAction::setEnabled);
#endif

    return child;
}

void MainWindow::createActions()
{
    //文件菜单
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    fileMenu->addSeparator();

    QMenu *recentMenu = fileMenu->addMenu(tr("Recent..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = fileMenu->addSeparator();

    setRecentFilesVisible(MainWindow::hasRecentFiles());

    fileMenu->addSeparator();

//! [0]
    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::closeAllWindows);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAct);
//! [0]

    //编辑菜单
#ifndef QT_NO_CLIPBOARD
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);
#endif

    //搜索菜单
    QMenu *searchMenu = menuBar()->addMenu(tr("&Search"));
    QToolBar *searchToolBar = addToolBar(tr("Search"));

    const QIcon findIcon = QIcon::fromTheme("edit-find", QIcon(":/images/find.ico"));
    findAct = new QAction(findIcon, tr("&FindAndReplace"), this);
    findAct->setShortcuts(QKeySequence::Find);
    findAct->setStatusTip(tr("Find the first occurrence of a string."));
    connect(findAct, &QAction::triggered, this, &MainWindow::findAndReplace);
    searchMenu->addAction(findAct);
    searchToolBar->addAction(findAct);

    findNextAct = new QAction( tr("&FindNext"), this);
    findNextAct->setShortcuts(QKeySequence::FindNext);
    findNextAct->setStatusTip(tr("Find next string."));
    connect(findNextAct, &QAction::triggered, this, &MainWindow::findNext);
    searchMenu->addAction(findNextAct);
    searchMenu->addSeparator();

    targetToLineAct = new QAction( tr("&Locate to line"), this);
    targetToLineAct->setShortcuts(QKeySequence::WhatsThis);
    targetToLineAct->setStatusTip(tr("Locate to line."));
    connect(targetToLineAct, &QAction::triggered, this, &MainWindow::locateToLine);
    searchMenu->addAction(targetToLineAct);

    //窗口菜单
    windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, &QAction::triggered,
            mdiArea, &QMdiArea::closeActiveSubWindow);

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, &QAction::triggered, mdiArea, &QMdiArea::closeAllSubWindows);

    showInExplorerAct = new QAction(tr("Show in &Explorer"), this);
    showInExplorerAct->setStatusTip(tr("Show file in Explorer"));
    connect(showInExplorerAct, &QAction::triggered, this, &MainWindow::showInExplorer);

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow);

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow);

    windowMenuSeparatorAct = new QAction(this);
    windowMenuSeparatorAct->setSeparator(true);

    updateWindowMenu();

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

MdiChild *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

//拖曳进入
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    //只允许接受合法url资源
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

//拖曳放下
void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
       return;
    foreach (const QUrl fileName , urls)
    {
        openFile(fileName.toLocalFile());
    }
}

//在资源管理器中显示
void MainWindow::showInExplorer()
{
    MdiChild* currMdiChild = activeMdiChild();
    if(currMdiChild == 0)
    {
        outPutConsole("【错误】没有找到activeMdiChild。");
        return;
    }
    if(currMdiChild->isUntitledFile())
    {
        outPutConsole("【错误】请先保存文件。");
        return;
    }
    QString filePath = QFileInfo(currMdiChild->currentFile()).absolutePath();

#if defined (_WIN32)
    ShellExecute(NULL, L"explore", filePath.toStdWString().c_str(),   NULL,   NULL,   SW_SHOW);
#else
    QDesktopServices::openUrl(QUrl(filePath, QUrl::TolerantMode));
#endif
}

//刷新函数列表，bClearOldList为true代表清理当前列表并更新为functionDetailList，bClearOldList为false表示只添加functionDetailList
void MainWindow::RefreshFuncList(const QList<func> *functionDetailList, bool bClearOldList)
{
    if(bClearOldList)
    {
        funcList->clear();
    }
    for (int i = 0 ; i < functionDetailList->size(); i++)
    {
        funcList->addItem( new QListWidgetItem ( QIcon(":/images/func.png"), functionDetailList->at(i).strFunction,funcList,functionDetailList->at(i).position));
    }
}

//点击函数列表
void MainWindow::funcListCliecked(QListWidgetItem *item)
{
    int nLine = item->type();
    item->setTextColor(0xfe0000);
    MdiChild* currMdiChild = activeMdiChild();
    if(currMdiChild == 0)
    {
        outPutConsole("【错误】没有找到activeMdiChild。");
        return;
    }
    currMdiChild->jumpToLine(nLine);
}

//打开查找和替换对话框
void MainWindow::findAndReplace()
{
    MdiChild *child = activeMdiChild();
    if (child)
    {
        findDlg->showWithInsertItem( child->selectedText());
    }
    else
    {
        findDlg->showNormal();
    }
}

//执行查找
void MainWindow::exeuteFind(const QString &expr, bool re, bool cs, bool wo,bool wrap, bool insection, bool next)
{
    MdiChild *child = activeMdiChild();
    if (child)
    {
        if(next)
        {
            if(insection)
            {
                child->findFirstInSelection(expr,re,cs,wo,next);
            }
            else
            {
                child->findFirst(expr,re,cs,wo,wrap,next);
            }
        }
        else
        {
            child->findFirst(expr,re,cs,wo,wrap,next);
            child->findNext();
        }
    }
}

//执行查找下一个
void MainWindow::findNext()
{
    MdiChild *child = activeMdiChild();
    if (child)
    {
        child->findNext();
    }
}

//执行替换
void MainWindow::exeuteReplace(const QString &exprFind,const QString &expr, bool re, bool cs, bool wo)
{
    MdiChild *child = activeMdiChild();
    if (child)
    {
        if(child->findFirst(exprFind,re,cs,wo,true))
        {
            child->replace(expr);
        }
    }
}

//执行替换所有
void MainWindow::exeuteReplaceAll(const QString &exprFind,const QString &expr, bool re, bool cs, bool wo)
{
    MdiChild *child = activeMdiChild();
    if (child)
    {
        int nCount = 0;
        while(child->findFirst(exprFind,re,cs,wo,false,true,0,0,false))
        {
            child->replace(expr);
            nCount++;
        }
        QString strMsg;
        strMsg.sprintf("replaced %d items.", nCount);
        statusBar()->showMessage(strMsg, 2000);
    }
}

//转到行
void MainWindow::locateToLine()
{
    MdiChild *child = activeMdiChild();
    if (child)
    {
        char szTips[64];
        sprintf(szTips,"Line Number(1-%d):",child->lines());

        bool ok = false;
        QString line = QInputDialog::getText(this,tr("Locate to line"),tr(szTips),QLineEdit::Normal,"",&ok);
        if(ok&&!line.isEmpty())
        {
            int nLine = line.toInt();
            if(nLine > 0)
            {
                child->jumpToLine( nLine-1);
            }
            else
            {
                child->jumpToLine(0);
            }
        }
    }
}

//刷新编码显示
void MainWindow::refreshEncodingDisplay(int nEncoding)
{
    int nDisplayIndex = 0;
    if(nEncoding >= 0 && nEncoding <= 3)
    {
        nDisplayIndex = nEncoding;
    }
    cbEncoding->setCurrentIndex(nDisplayIndex);
}

//检测窗口激活事件
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if( watched == this )
    {
        //窗口激活
        if( QEvent::WindowActivate == event->type())
        {
            foreach (QMdiSubWindow *window, mdiArea->subWindowList())
            {
                MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
                if (mdiChild)
                {
                    if(mdiChild->checkReloadFile())
                    {
                        outPutConsole("file maybe changed by others, reload it.");
                        mdiChild->loadFile(mdiChild->currentFile());
                    }
                }
            }
            bActiveWindow = true;
            return true ;
        }
        else if( QEvent::WindowDeactivate == event->type())
        {
            bActiveWindow = false;
            return true;
        }
        else
        {
            return false ;
        }
    }
    return false ;
}

//启动时判断是否需要显示界面还是激活另外的进程(返回true，第一次启动，显示界面； false 后续的启动，直接退出)
bool MainWindow::checkAndOpenFile( const QString &openFileName )
{
    //多个lua_debug_ui进程通信手段
    hSharedMMap = NULL;
    hSharedMMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, SHARED_MEM_NAME);
    if (NULL == hSharedMMap)
    {
        //打开失败，创建
        hSharedMMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,
                                    NULL,
                                    PAGE_READWRITE,
                                    0,
                                    MAX_PATH*2,
                                    SHARED_MEM_NAME);
        if(NULL == hSharedMMap)
        {
            QMessageBox::information(NULL, "lua_debug_ui", "CreateFileMapping fail!",  QMessageBox::Ok , QMessageBox::Ok);
            return false;
        }
        pSharedMBuffer = (char*)::MapViewOfFile(hSharedMMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        return true;
    }
    else
    {
        //打开成功，说明不是第一次了
        LPVOID pBuffer = ::MapViewOfFile(hSharedMMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        QByteArray byteBuf = openFileName.toUtf8();
        strcpy((char*)pBuffer, byteBuf.constData());
        ::UnmapViewOfFile(pBuffer);
        ::CloseHandle(hSharedMMap);
        return false;
    }
}

//定时读取共享内存
void MainWindow::timerEvent( QTimerEvent *event )
{
    if(event->timerId() == timerShardMId)
    {
        if(NULL == hSharedMMap)
        {
            return;
        }
        if(bActiveWindow)
        {
            return;
        }

        if(pSharedMBuffer[0] != 0)
        {
            openFile(pSharedMBuffer);
            pSharedMBuffer[0] = 0;

            //激活到前台
            Qt::WindowStates winStatus = Qt::WindowNoState;
            if (windowState() & Qt::WindowMaximized)
            {
                winStatus = Qt::WindowMaximized;
            }
            setWindowState(Qt::WindowMinimized);
            setWindowState(Qt::WindowActive | winStatus);
            activateWindow();
            raise();
            bActiveWindow = true;
        }
    }
}
