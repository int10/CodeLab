#-------------------------------------------------
#
# Project created by QtCreator 2013-12-30T20:16:46
#
#-------------------------------------------------

QT       += core gui

TARGET = MfcTest
TEMPLATE = app

LIBS += User32.LIB Gdi32.lib
SOURCES += main.cpp\
        mainwindow.cpp \
    remotecontrol.cpp \
    iniparse.cpp \
    common.cpp \
    common.cpp \
    DocumentMgr.cpp

HEADERS  += mainwindow.h \
    remotecontrol.h \
    iniparse.h \
    common.h \
    common.h \
    DocumentMgr.h

FORMS    += mainwindow.ui
