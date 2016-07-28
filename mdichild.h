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

#ifndef MDICHILD_H
#define MDICHILD_H

#define QSCINTILLA_DLL

#include <QTextEdit>
#include <QMainWindow>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscistyle.h>
#include <Qsci/qscilexer.h>
#include <Qsci/qscilexercmake.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexercsharp.h>
#include <Qsci/qscilexercss.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerjava.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerruby.h>
#include <Qsci/qscilexersql.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qsciabstractapis.h>
#include <Qsci/qsciapis.h>


#define LUA_MARK_BREAK_POINT    0
#define LUA_MARK_BREAK_ARROW    1

//函数对象，保存了函数名称和函数位置
class func
{
public:
    func():strFunction(""),position(0) {}
    QString strFunction;
    int     position;
};

class MainWindow;
class  MdiChild : public QsciScintilla
{
    Q_OBJECT

public:
    MdiChild(MainWindow &parent);
    ~MdiChild();
    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFileName; }
    bool    isUntitledFile(){return isUntitled;}
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
private slots:
    void documentWasModified();
    void marginClicked(int margin, int line, Qt::KeyboardModifiers state);
    void charAddedAndAutoComplete(int charAdd);
    void selectedHighLight();
private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void RefreshFuncList(bool bForceRefreshFuncList);
    int  ConvertPosToLine(int nPos);
    void ChangeLexer(const QString &fileName);

    QString curFileName;            //当前文件全路径
    bool    isUntitled;             //是否为无标题文本
    MainWindow  &mainFrame;         //保存的主界面指针
    QsciLexer   *Lexer;             //当前文档的语法解析器
    QRegularExpression regexFunc;   //提取函数列表的正则表达式
    int lastInputChar;              //记录最后一个输入的字符

    //两份函数列表，相互轮转作为缓存
    QList<func> FuncList1;
    QList<func> FuncList2;
    bool bFunc1IsCache;
    inline QList<func>* getCurrentFuncList(){ return (bFunc1IsCache ? (&FuncList2): (&FuncList1)); }
    inline QList<func>* getCacheFuncList()  { return (bFunc1IsCache ? (&FuncList1): (&FuncList2)); }
    inline void  swapCurrentAndCache()      { bFunc1IsCache = (bFunc1IsCache ? false : true );}
};

#endif
