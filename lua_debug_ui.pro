QT += widgets

HEADERS       = mainwindow.h \
                mdichild.h \
    dialogfind.h

SOURCES       = main.cpp \
                mainwindow.cpp \
                mdichild.cpp \
    dialogfind.cpp

RESOURCES     = mdi.qrc

LIBS 	+=  ../Qsci/qscintilla2.lib

FORMS += \
    dialogfind.ui

