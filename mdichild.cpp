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
    isUntitled = true;
    Lexer = NULL;
    lastInputChar = 0;
    bFunc1IsCache = false;
    regexFunc.setPattern("function\\s+(.+)?\\(");
    regexFunc.optimize();

    //��������ҷ�ļ����༭��ֻ������ҷ��������
    setAcceptDrops(false);

    //�趨ѡ���и���
    setCaretLineVisible(true);

    //�趨tab����
    setTabWidth(4);

    //�Զ�ƥ������
    setBraceMatching(QsciScintilla::SloppyBraceMatch);

    //�༭����ʾ����
    SendScintilla(QsciScintillaBase::SCI_SETCODEPAGE,QsciScintillaBase::SC_CP_UTF8);

    //ҳ������
    setMarginsFont(QFont("Consolas"));
    setMarginsForegroundColor(0xd5a476);

    //�趨�к�
    setMarginLineNumbers(1,true);
    setMarginWidth(1,55);
    setMarginSensitivity(1,true);

    //�趨�۵�
    setFolding(QsciScintilla::CircledTreeFoldStyle, 2);
    setMarginSensitivity(2,true);

    //�Զ�����
    setAutoIndent(true);
    setTabIndents(true);
    setIndentationGuides(true);

    //�ϵ����
    markerDefine(QsciScintilla::Circle,LUA_MARK_BREAK_POINT);
    setMarkerBackgroundColor(0xff005f,LUA_MARK_BREAK_POINT);

    //���ӻ�ɾ���ϵ㣬��ݼ�F9
    connect(this, SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)),this, SLOT(marginClicked(int, int, Qt::KeyboardModifiers)));

    //���Լ�ͷ����
    markerDefine(QsciScintilla::RightArrow,LUA_MARK_BREAK_ARROW);
    setMarkerBackgroundColor(0x00f05f,LUA_MARK_BREAK_ARROW);

    //�Զ���ɣ���ݼ�Alt+/
    setAutoCompletionSource(QsciScintilla::AcsAll);
    connect(this,SIGNAL(SCN_CHARADDED(int)),this,SLOT(charAddedAndAutoComplete(int)));

    //ѡ���ı�ʱ�Զ����Ҳ�������Ӧ����ͬ�ı�
    connect(this,SIGNAL(selectionChanged()),this,SLOT(selectedHighLight()));

    //�Զ���ʾ
    setAnnotationDisplay(QsciScintilla::AnnotationBoxed);

}

//ѡ���ı�ʱ�Զ����Ҳ�������Ӧ����ͬ�ı�
void MdiChild::selectedHighLight()
{

}

//ֻ�����Զ����.charAdd���<0�������û���ı�׷��
void MdiChild::charAddedAndAutoComplete(int charAdd)
{
    //��CharAdded�¼��м�¼���������ַ�
    lastInputChar = charAdd;

    this->autoCompleteFromAll();
}

//�����ڲ�����ĺ����б����ж��ǲ�����Ҫˢ�½���ĺ����б�
void MdiChild::RefreshFuncList(bool bForceRefreshFuncList)
{
    //���º����б�
    QList<func> * currFuncList = getCurrentFuncList();
    QList<func> * cacheFuncList = getCacheFuncList();
    currFuncList->clear();
    func tmpFunc;

    QRegularExpressionMatchIterator iterator = regexFunc.globalMatch(text());
    while (iterator.hasNext())
    {
        QRegularExpressionMatch match = iterator.next();

        //��ǰ����ڱ�BLOCK�ڵ����λ��
        tmpFunc.strFunction = match.captured(1);
        tmpFunc.position =  ConvertPosToLine(match.capturedStart(1));
        currFuncList->append(tmpFunc);
    }

    //�����Ҫǿ��ˢ�£���ǿ��ˢ��
    if(bForceRefreshFuncList)
    {
        mainFrame.RefreshFuncList(currFuncList,true);
        goto UPDATE_CACHE;
    }

    //����Ҫǿ��ˢ�µ�����£�����ǰ�ĺ����б��뻺��ĺ����б��Ƚϣ����Ƿ�����ˢ��
    int cacheListLen = cacheFuncList->size();
    int currListLen = currFuncList->size();
    if(currListLen < cacheListLen)
    {
        //�����б�������ɾ�������������б�����ȫ�����������
        mainFrame.RefreshFuncList(currFuncList,true);
        goto UPDATE_CACHE;
    }
    else
    {
        //���������Ӷ������޸Ķ��������޶���
        for(int i = 0 ; i < cacheListLen ; i++)
        {
            if( (currFuncList->at(i).strFunction == cacheFuncList->at(i).strFunction ) &&
                (currFuncList->at(i).position == cacheFuncList->at(i).position ) )
            {
                continue;
            }

            //�������޸Ķ����������б�����ȫ�����������
            mainFrame.RefreshFuncList(currFuncList,true);
            goto UPDATE_CACHE;
        }

        //currListLenΪcachedFuncList���Ӽ�
        if(currListLen == cacheListLen)
        {
            //�޶����������б���������ˢ�£�
        }
        else
        {
            //���������Ӷ����������б�����ֻ��Ҫ���������ӵĺ����ľͿ����ˣ�
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
    //�����б��͵�ǰ�б�����λ��
    swapCurrentAndCache();
}

//ֻ������ݼ�
void MdiChild::keyPressEvent(QKeyEvent *event)
{
    int line,index;
    switch(event->key())
    {
        case Qt::Key_F9:
        {
            //���ӻ�ɾ���ϵ�
            getCursorPosition(&line, &index);
            marginClicked(LUA_MARK_BREAK_POINT, line, event->modifiers());
            return;
        }
        case Qt::Key_F12:
        {
            //��ת������
//            char szerr[10000];
//            QPoint curPos = cursor().pos();
//            QPoint winPos = QWidget::mapFromGlobal(curPos) ;
//            int ff = SendScintilla(SCI_POSITIONFROMPOINTCLOSE,winPos.x(),winPos.y());
//            sprintf(szerr,"%d",ff);
//            mainFrame.outPutConsole(szerr);
            findFirst(selectedText(),false,true,false,true);


            return;
        }
        case Qt::Key_Slash:
        {
            if(event->modifiers() == Qt::AltModifier)
            {
                //�Զ����
                charAddedAndAutoComplete(-1);
                return;
            }
            break;
        }
    default:
        break;
    }

    //������������������ϲ�
    QsciScintilla::keyPressEvent(event);
}

void MdiChild::mouseMoveEvent(QMouseEvent *e)
{
    char szerr[10000];
    sprintf(szerr,"g:%d-%d",e->globalX(),e->globalY());
    mainFrame.outPutConsole(szerr);

    QPoint winPos = QWidget::mapFromGlobal(e->globalPos()) ;
    sprintf(szerr,"w:%d-%d",winPos.x(),winPos.y());
    mainFrame.outPutConsole(szerr);

    QsciScintilla::mouseMoveEvent(e);
}

bool MdiChild::event(QEvent *e)
{

   return QsciScintilla::event(e);
}

//���ҳ�����ӻ�ɾ���ϵ�
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
    curFileName = tr("NewFile%1.lua").arg(sequenceNumber++);
    setWindowTitle(curFileName + "[*]");
    connect(this, SIGNAL(textChanged()),this, SLOT(documentWasModified()));

    ChangeLexer("");
}

bool MdiChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("LuaStudio"),tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    connect(this, SIGNAL(textChanged()),this, SLOT(documentWasModified()));

    ChangeLexer(fileName);
    return true;
}

bool MdiChild::save()
{
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
        QMessageBox::warning(this, tr("LuaStudio"), tr("Cannot write file %1:\n%2.") .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << this->text() ;
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
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
    QsciScintilla::focusInEvent(event);
}

void MdiChild::documentWasModified()
{
    setWindowModified(this->isModified());
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

//�����﷨������
void MdiChild::ChangeLexer(const QString &fileName)
{
    if("" == fileName)
    {
         Lexer = new QsciLexerLua;
    }
    else
    {
        QString fileNameNoPath  = QFileInfo(fileName).fileName();
        QString suffix = QFileInfo(fileName).completeSuffix();
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
    setAutoCompletionCaseSensitivity(false);
}

//������λ��ת�����к�
int MdiChild::ConvertPosToLine(int nPos)
{
    const QString& strText = text();
    int nLineAtPos = 0;
    for(int i = 0 ; i < nPos; i++)
    {
        if(strText[i] == '\n')
        {
            nLineAtPos++;
        }
    }
    return nLineAtPos;
}