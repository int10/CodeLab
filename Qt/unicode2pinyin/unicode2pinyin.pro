#-------------------------------------------------
#
# Project created by QtCreator 2015-08-17T13:57:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sql
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Sqlite.cpp \
    sqlite3.c

HEADERS  += mainwindow.h \
    sqlite3ext.h \
    sqlite3.h \
    Sqlite.h

FORMS    += mainwindow.ui
