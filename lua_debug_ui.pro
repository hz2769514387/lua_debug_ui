QT += widgets

HEADERS       = mainwindow.h \
                mdichild.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                mdichild.cpp
RESOURCES     = mdi.qrc

LIBS 	+= ../Qsci/qscintilla2.lib

