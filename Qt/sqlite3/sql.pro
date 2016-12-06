#-------------------------------------------------
#
# Project created by QtCreator 2016-06-20T00:12:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sql
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sqlite3.c

HEADERS  += mainwindow.h \
    sqlite3.h \
    sqlite3ext.h

FORMS    += mainwindow.ui
