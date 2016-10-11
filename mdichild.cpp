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

#include "mdichild.h"
#include "mainwindow.h"

MdiChild::~MdiChild()
{
    delete Lexer;
}

MdiChild::MdiChild(MainWindow &parent)
    :mainFrame(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    curFileEncode = 0;
    isUntitled = true;
    Lexer = NULL;
    lastInputChar = 0;
    bFunc1IsCache = false;
    //sRegexFunc = textAsBytes("function\\s+(.+)?\\(");
    sRegexFunc = textAsBytes("function\\s+.+?\(");

    //不允许拖曳文件到编辑框，只允许拖曳到主窗口
    setAcceptDrops(false);

    //设定选中行高亮
    setCaretLineVisible(true);

    //设定tab宽度
    setTabWidth(4);

    //自动匹配括号
    setBraceMatching(QsciScintilla::SloppyBraceMatch);

    //编辑器显示中文
    SendScintilla(QsciScintillaBase::SCI_SETCODEPAGE,QsciScintillaBase::SC_CP_UTF8);

    //页边属性
    setMarginsFont(QFont("Consolas"));
    setMarginsForegroundColor(0xd5a476);

    //设定行号
    setMarginLineNumbers(1,true);
    setMarginWidth(1,55);
    setMarginSensitivity(1,true);

    //设定折叠
    setFolding(QsciScintilla::BoxedTreeFoldStyle, 2);
    setMarginSensitivity(2,true);

    //自动缩进
    setAutoIndent(true);
    setTabIndents(true);
    setIndentationGuides(true);

    //断点符号
    markerDefine(QsciScintilla::Circle,LUA_MARK_BREAK_POINT);
    setMarkerBackgroundColor(0xff005f,LUA_MARK_BREAK_POINT);

    //添加或删除断点，快捷键F9
    connect(this, SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)),this, SLOT(marginClicked(int, int, Qt::KeyboardModifiers)));

    //调试箭头符号
    markerDefine(QsciScintilla::RightArrow,LUA_MARK_BREAK_ARROW);
    setMarkerBackgroundColor(0x00f05f,LUA_MARK_BREAK_ARROW);

    //自动完成，快捷键Alt+/
    setAutoCompletionSource(QsciScintilla::AcsAll);
    connect(this,SIGNAL(SCN_CHARADDED(int)),this,SLOT(charAddedAndAutoComplete(int)));

    //选择文本时自动查找并高亮对应的相同文本
    connect(this,SIGNAL(selectionChanged()),this,SLOT(selectedHighLight()));
    indicatorDefine(QsciScintilla::RoundBoxIndicator, INDICATOR_SELECTED);

    //按下control+鼠标点击时标记当前选择单词
    indicatorDefine(QsciScintilla::PlainIndicator, INDICATOR_CONTROL);

    //自动提示
    setAnnotationDisplay(QsciScintilla::AnnotationBoxed);

    //文档改变时的判断

}

//选择文本时自动查找并高亮对应的相同文本
void MdiChild::selectedHighLight()
{
    QString &strSel = selectedText();
    if(strSel.isEmpty())
    {
       int endline,endindex;
       lineIndexFromPosition(SendScintilla(SCI_GETLENGTH), &endline, &endindex);
       clearIndicatorRange(0,0,endline,endindex,INDICATOR_SELECTED);
       return;
    }

    //寻找所有跟选择文本相同的位置并标记为indicator INDICATOR_SELECTED
    int startPos = 0;
    while(true)
    {
        int endPos = SendScintilla(SCI_GETLENGTH);
        SendScintilla(SCI_SETSEARCHFLAGS, SCFIND_MATCHCASE|SCFIND_WHOLEWORD);
        SendScintilla(SCI_SETTARGETSTART, startPos);
        SendScintilla(SCI_SETTARGETEND, endPos);

        ScintillaBytes s = textAsBytes(strSel);
        int pos = SendScintilla(SCI_SEARCHINTARGET, s.length(),ScintillaBytesConstData(s));
        if(-1 == pos)
        {
            //没找到
            return;
        }
        long targstart = SendScintilla(SCI_GETTARGETSTART);
        long targend = SendScintilla(SCI_GETTARGETEND);
        int startline,startindex,endline,endindex;
        lineIndexFromPosition(targstart, &startline, &startindex);
        lineIndexFromPosition(targend, &endline, &endindex);
        fillIndicatorRange(startline, startindex, endline,endindex, INDICATOR_SELECTED);

        startPos = targend;
    }
}

//只处理自动完成.charAdd如果<0，则代表没有文本追加
void MdiChild::charAddedAndAutoComplete(int charAdd)
{
    //在CharAdded事件中记录最后输入的字符
    lastInputChar = charAdd;

    this->autoCompleteFromAll();
}

//更新内部保存的函数列表，判断是不是需要刷新界面的函数列表
void MdiChild::RefreshFuncList(bool bForceRefreshFuncList)
{
    //初始化当前函数列表
    QList<func> * currFuncList = getCurrentFuncList();
    QList<func> * cacheFuncList = getCacheFuncList();
    currFuncList->clear();
    func tmpFunc;

    //根据正则表达式查找函数列表
    int startPos = 0;
    while(true)
    {
        int endPos = SendScintilla(SCI_GETLENGTH);
        SendScintilla(SCI_SETSEARCHFLAGS, SCFIND_MATCHCASE|SCFIND_REGEXP|SCFIND_CXX11REGEX);
        SendScintilla(SCI_SETTARGETSTART, startPos);
        SendScintilla(SCI_SETTARGETEND, endPos);

        int pos = SendScintilla(SCI_SEARCHINTARGET, sRegexFunc.length(),ScintillaBytesConstData(sRegexFunc));
        if(-1 == pos)
        {
            //没找到
            break;
        }
        long targstart = SendScintilla(SCI_GETTARGETSTART);
        long targend = SendScintilla(SCI_GETTARGETEND);
        QString strFunction = getTextRange(targstart,targend);
        int startline,startindex;
        lineIndexFromPosition(targstart, &startline, &startindex);
        tmpFunc.strFunction = strFunction;
        tmpFunc.position =  startline;
        currFuncList->append(tmpFunc);

        startPos = targend;
    }

    //如果需要强制刷新，则强制刷新
    if(bForceRefreshFuncList)
    {
        mainFrame.RefreshFuncList(currFuncList,true);
        goto UPDATE_CACHE;
    }

    //不需要强制刷新的情况下，将当前的函数列表与缓存的函数列表比较，看是否发生了刷新
    int cacheListLen = cacheFuncList->size();
    int currListLen = currFuncList->size();
    if(currListLen < cacheListLen)
    {
        //函数列表发生了删除动作（函数列表界面全部清掉重来）
        mainFrame.RefreshFuncList(currFuncList,true);
        goto UPDATE_CACHE;
    }
    else
    {
        //发生了添加动作或修改动作，或无动作
        for(int i = 0 ; i < cacheListLen ; i++)
        {
            if( (currFuncList->at(i).strFunction == cacheFuncList->at(i).strFunction ) &&
                (currFuncList->at(i).position == cacheFuncList->at(i).position ) )
            {
                continue;
            }

            //发生了修改动作（函数列表界面全部清掉重来）
            mainFrame.RefreshFuncList(currFuncList,true);
            goto UPDATE_CACHE;
        }

        //currListLen为cachedFuncList的子集
        if(currListLen == cacheListLen)
        {
            //无动作（函数列表界面无需刷新）
        }
        else
        {
            //发生了添加动作（函数列表界面只需要添加新增加的函数的就可以了）
            QList<func> newList;
            func newTmpFunc;
            for(int i = cacheListLen ; i < currListLen ; i++)
            {
                newTmpFunc.strFunction = currFuncList->at(i).strFunction;
                newTmpFunc.position = currFuncList->at(i).position;
                newList.append(newTmpFunc);
            }
            mainFrame.RefreshFuncList(&newList,false);
        }
        goto UPDATE_CACHE;
    }


UPDATE_CACHE:
    //缓存列表和当前列表交换位置
    swapCurrentAndCache();
}

//只处理快捷键
void MdiChild::keyPressEvent(QKeyEvent *event)
{
    int line,index;
    switch(event->key())
    {
        case Qt::Key_F9:
        {
            //添加或删除断点
            getCursorPosition(&line, &index);
            marginClicked(LUA_MARK_BREAK_POINT, line, event->modifiers());
            return;
        }
        case Qt::Key_F12:
        {
            //跳转到定义
            getCursorPosition(&line, &index);
            QString & strWordsAtPoint = wordAtLineIndex( line,  index);
            mainFrame.outPutConsole(strWordsAtPoint);
            return;
        }
        case Qt::Key_Slash:
        {
            if(event->modifiers() == Qt::AltModifier)
            {
                //自动完成
                charAddedAndAutoComplete(-1);
                return;
            }
        }
    default:
        break;
    }

    //不处理这个键，丢给上层
    QsciScintilla::keyPressEvent(event);
}

//点击鼠标事件，如果点击时按下了control则跳转到定义
void MdiChild::mousePressEvent(QMouseEvent *event)
{
    if ( Qt::ControlModifier == event->modifiers())
    {
        QPoint currTabPos = QWidget::mapFromGlobal(event->globalPos()) ;
        QString &strWordsAtPoint = wordAtPoint(currTabPos);
        mainFrame.outPutConsole(strWordsAtPoint);

        return;
    }

    QsciScintilla::mousePressEvent(event);
}

//点击页面添加或删除断点
void MdiChild::marginClicked(int margin, int line, Qt::KeyboardModifiers state)
{
    if (markersAtLine(line) )
    {
        markerDelete(line, LUA_MARK_BREAK_POINT);
    }
    else
    {
        markerAdd(line, LUA_MARK_BREAK_POINT);
    }
}

void MdiChild::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFileEncode = 0;
    mainFrame.refreshEncodingDisplay(curFileEncode);

    curFileName = tr("NewFile%1.lua").arg(sequenceNumber++);
    setWindowTitle(curFileName + "[*]");
    connect(this, SIGNAL(textChanged()),this, SLOT(documentWasModified()));

    ChangeLexer("");
}

bool MdiChild::loadFile(const QString &fileName)
{
    //文件编码识别
    if(false == guessFileEncoding(fileName))
    {
        return false;
    }

    //真正打开文件
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        mainFrame.outPutConsole(tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setText(in.readAll());
    QApplication::restoreOverrideCursor();
    file.close();

    setCurrentFile(fileName);
    connect(this, SIGNAL(textChanged()),this, SLOT(documentWasModified()));

    ChangeLexer(fileName);

    //保存点
    SendScintilla(SCI_SETSAVEPOINT);
    curFileModifyTime = QFileInfo(fileName).lastModified();
    return true;
}

bool MdiChild::save()
{
    //保存点
    SendScintilla(SCI_SETSAVEPOINT);

    if (isUntitled)
    {
        return saveAs();
    }
    else
    {
        return saveFile(curFileName);
    }
    return  true;
}

bool MdiChild::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),curFileName);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MdiChild::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("lua_debug_ui"), tr("Cannot write file %1:\n%2.") .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << this->text();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    curFileModifyTime = QFileInfo(fileName).lastModified();
    mainFrame.outPutConsole("save file time :" + curFileModifyTime.toString());
    return true;
}

QString MdiChild::userFriendlyCurrentFile()
{
    return strippedName(curFileName);
}

void MdiChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MdiChild::focusInEvent(QFocusEvent *event)
{
    RefreshFuncList(true);
    mainFrame.refreshEncodingDisplay(curFileEncode);

    QsciScintilla::focusInEvent(event);
}

void MdiChild::documentWasModified()
{
    setWindowModified(isModified());
    RefreshFuncList(false);
}

bool MdiChild::maybeSave()
{
    if (!isModified())
    {
        return true;
    }

    const QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("LuaStudio"),
                                   tr("'%1' has been modified.\nDo you want to save your changes?").arg(userFriendlyCurrentFile()),
                                   QMessageBox::Save | QMessageBox::Discard| QMessageBox::Cancel);
    switch (ret)
    {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MdiChild::setCurrentFile(const QString &fileName)
{
    curFileName = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MdiChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

//设置语法解析器
void MdiChild::ChangeLexer(const QString &fileName)
{
    if("" == fileName)
    {
         Lexer = new QsciLexerLua;
    }
    else
    {
        QString fileNameNoPath  = QFileInfo(fileName).fileName();
        QString suffix = QFileInfo(fileName).suffix();
        if(suffix.compare("lua", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerLua;
        }
        else if((fileNameNoPath.compare("CMakeLists.txt", Qt::CaseSensitivity::CaseInsensitive) == 0)    ||
                (suffix.compare("cmake", Qt::CaseSensitivity::CaseInsensitive) == 0))
        {
            Lexer = new QsciLexerCMake;
        }
        else if((suffix.compare("cpp", Qt::CaseSensitivity::CaseInsensitive) == 0) ||
                (suffix.compare("cxx", Qt::CaseSensitivity::CaseInsensitive) == 0)  ||
                (suffix.compare("c", Qt::CaseSensitivity::CaseInsensitive) == 0)  ||
                (suffix.compare("h", Qt::CaseSensitivity::CaseInsensitive) == 0)  ||
                (suffix.compare("hpp", Qt::CaseSensitivity::CaseInsensitive) == 0))
        {
            Lexer = new QsciLexerCPP;
        }
        else if(suffix.compare("cs", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerCSharp;
        }
        else if(suffix.compare("css", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerCSS;
        }
        else if((suffix.compare("html", Qt::CaseSensitivity::CaseInsensitive) == 0) ||
                (suffix.compare("htm", Qt::CaseSensitivity::CaseInsensitive) == 0)  ||
                (suffix.compare("jsp", Qt::CaseSensitivity::CaseInsensitive) == 0)  ||
                (suffix.compare("asp", Qt::CaseSensitivity::CaseInsensitive) == 0))
        {
            Lexer = new QsciLexerHTML;
        }
        else if(suffix.compare("java", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerJava;
        }
        else if(suffix.compare("js", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerJavaScript;
        }
        else if(fileNameNoPath.compare("Makefile", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerMakefile;
        }
        else if(suffix.compare("pl", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerPerl;
        }
        else if(suffix.compare("py", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerPython;
        }
        else if(suffix.compare("rb", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerRuby;
        }
        else if(suffix.compare("sql", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerSQL;
        }
        else if(suffix.compare("xml", Qt::CaseSensitivity::CaseInsensitive) == 0)
        {
            Lexer = new QsciLexerXML;
        }
        else
        {
            Lexer = new QsciLexerLua;
        }
    }

    setLexer(Lexer);
    Lexer->setFont(QFont("Consolas"));
    Lexer->setAutoIndentStyle(QsciScintilla::AiMaintain | QsciScintilla::AiOpening | QsciScintilla::AiClosing);
    //setAutoCompletionCaseSensitivity(false);
}

//获取对应位置的文本
QString MdiChild::getTextRange(int start_pos,int end_pos) const
{
    if (start_pos < 0 || start_pos <0 )
        return QString();

    int word_len = end_pos - start_pos;
    if (word_len <= 0)
        return QString();

    char *buf = new char[word_len + 1];
    SendScintilla(SCI_GETTEXTRANGE, start_pos, end_pos, buf);
    QString word = bytesAsText(buf);
    delete[] buf;
    return word;
}

//跳转到指定行
void MdiChild::jumpToLine(int line)
{
    int pos = positionFromLineIndex(line,0);

    SendScintilla(SCI_ENSUREVISIBLEENFORCEPOLICY, line);

    // Now set the selection.
    SendScintilla(SCI_SETSEL, pos, pos);
}

//猜测并识别文件编码
bool  MdiChild::guessFileEncoding(const QString &fileName)
{
    //猜测并识别文件编码
    QFile fileBytes(fileName);
    if (!fileBytes.open(QFile::ReadOnly ))
    {
        mainFrame.outPutConsole(tr("Cannot read file %1:\n%2.").arg(fileName).arg(fileBytes.errorString()));
        return false;
    }
    unsigned short codeType = 0;
    fileBytes.read((char*)&codeType,sizeof(codeType));
    if(0xfeff == codeType)
    {
        curFileEncode = 1;
    }
    else if(0xfffe == codeType)
    {
        curFileEncode = 2;
    }
    else if(0xbbef == codeType)
    {
        curFileEncode = 3;
    }
    else
    {
        curFileEncode = 0;
    }
    fileBytes.close();
    mainFrame.refreshEncodingDisplay(curFileEncode);
    return  true;
}

//检查是否需要重新加载文件
bool  MdiChild::checkReloadFile()
{
    if(isUntitled)
    {
        return false;
    }
    QDateTime lastModifTime = QFileInfo(curFileName).lastModified();

    if ( curFileModifyTime.offsetFromUtc() != lastModifTime.offsetFromUtc())
    {
        mainFrame.outPutConsole("saved modif time: " + curFileModifyTime.toString() + ", last modif time:" + lastModifTime.toString());
        return true;
    }
    return false;
}
