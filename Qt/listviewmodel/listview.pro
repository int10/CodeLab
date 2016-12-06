#-------------------------------------------------
#
# Project created by QtCreator 2015-04-26T01:10:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = listview
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ImeiListModel.cpp \
    ImeiListDelegate.cpp

HEADERS  += mainwindow.h \
    ImeiListModel.h \
    ImeiListDelegate.h

FORMS    += mainwindow.ui
