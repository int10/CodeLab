#-------------------------------------------------
#
# Project created by QtCreator 2016-04-15T00:37:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = timer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Thread.cpp

HEADERS  += mainwindow.h \
    Thread.h

FORMS    += mainwindow.ui
