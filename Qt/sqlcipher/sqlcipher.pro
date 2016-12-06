#-------------------------------------------------
#
# Project created by QtCreator 2016-11-11T16:50:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sq
TEMPLATE = app

LIBS += -L$$PWD/Lib
LIBS += -llibeay32 -lssleay32

INCLUDEPATH += ./ ./openssl ./sqlite ./sqlcipher

SOURCES += main.cpp\
        mainwindow.cpp \
    Sqlite.cpp \
    sqlite/sqlite3.c \
    sqlcipher/crypto.c \
    sqlcipher/crypto_impl.c \
    sqlcipher/pager.c

HEADERS  += mainwindow.h \
    Sqlite.h \
    sqlite/sqlite3.h \
    sqlite/sqlite3ext.h \
    sqlcipher/crypto.h

FORMS    += mainwindow.ui
