#-------------------------------------------------
#
# Project created by QtCreator 2017-01-17T11:32:26
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = thunder
TEMPLATE = app

LIBS += -lAdvAPI32 -lsetupapi -lUser32 -lshlwapi

SOURCES += main.cpp\
        mainwindow.cpp \
    DownLoader.cpp \
    DownLoaderBase.cpp \
    DownLoaderForThunder.cpp \
    ThunderDownLoad.cpp

HEADERS  += mainwindow.h \
    DownLoader.h \
    DownLoaderBase.h \
    DownLoaderForThunder.h \
    DownWrapper.h \
    ThunderDownLoad.h \
    xldl.h

FORMS    += mainwindow.ui
